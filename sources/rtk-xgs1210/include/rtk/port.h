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
 * $Revision: 98657 $
 * $Date: 2019-07-12 15:32:48 +0800 (Fri, 12 Jul 2019) $
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 *           (2) UDLD
 *           (3) RLDP
 */

#ifndef __RTK_PORT_H__
#define __RTK_PORT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chipdef.h>

/*
 * Symbol Definition
 */
#define RTK_PHY_MAC_INTF_SDS_MAX        (2)


/*
 * Data Declaration
 */
typedef enum rtk_port_linkStatus_e
{
    PORT_LINKDOWN = 0,
    PORT_LINKUP,
    PORT_LINKSTATUS_END
} rtk_port_linkStatus_t;

typedef enum rtk_port_duplex_e
{
    PORT_HALF_DUPLEX = 0,
    PORT_FULL_DUPLEX,
    PORT_DUPLEX_END
} rtk_port_duplex_t;

typedef enum rtk_port_speed_e
{
    PORT_SPEED_10M = 0,
    PORT_SPEED_100M,
    PORT_SPEED_1000M,
    PORT_SPEED_500M,
    PORT_SPEED_2G,   /* Applicable to 8380 */
    PORT_SPEED_2_5G,
    PORT_SPEED_5G,
    PORT_SPEED_10G,
    PORT_SPEED_END
} rtk_port_speed_t;

typedef enum rtk_port_crossOver_mode_e
{
    PORT_CROSSOVER_MODE_AUTO = 0,
    PORT_CROSSOVER_MODE_MDI,
    PORT_CROSSOVER_MODE_MDIX,
    PORT_CROSSOVER_MODE_END
} rtk_port_crossOver_mode_t;

typedef enum rtk_port_crossOver_status_e
{
    PORT_CROSSOVER_STATUS_MDI = 0,
    PORT_CROSSOVER_STATUS_MDIX,
    PORT_CROSSOVER_STATUS_END
} rtk_port_crossOver_status_t;


typedef enum rtk_port_masterSlave_e
{
    PORT_AUTO_MODE = 0,
    PORT_SLAVE_MODE,
    PORT_MASTER_MODE,
    PORT_MASTER_SLAVE_END
} rtk_port_masterSlave_t;

typedef enum rtk_port_localPacket_action_e
{
    PORT_LOCAL_PACKET_DROP = 0,
    PORT_LOCAL_PACKET_FORWARD,
    PORT_LOCAL_PACKET_END
} rtk_port_localPacket_action_t;

typedef enum rtk_port_phy_reg_e
{
    PHY_REG_CONTROL             = 0,
    PHY_REG_STATUS,
    PHY_REG_IDENTIFIER_1,
    PHY_REG_IDENTIFIER_2,
    PHY_REG_AN_ADVERTISEMENT,
    PHY_REG_AN_LINKPARTNER,
    PHY_REG_1000_BASET_CONTROL  = 9,
    PHY_REG_1000_BASET_STATUS,
    PHY_REG_END                 = 32
} rtk_port_phy_reg_t;

typedef struct rtk_port_phy_ability_s
{
    uint32 Half_10:1;
    uint32 Full_10:1;
    uint32 Half_100:1;
    uint32 Full_100:1;
    uint32 Half_1000:1;
    uint32 Full_1000:1;
#if (defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300))
    uint32 Half_10G:1;
    uint32 Full_10G:1;
#endif
    uint32 adv_2_5G:1;
    uint32 adv_5G:1;
    uint32 adv_10GBase_T:1;
    uint32 FC:1;
    uint32 AsyFC:1;
} rtk_port_phy_ability_t;

typedef struct rtk_port_flowctrl_mode_s
{
    rtk_enable_t        rx_pause;
    rtk_enable_t        tx_pause;
} rtk_port_flowctrl_mode_t;

typedef enum rtk_port_vlanIsolationSrc_e
{
    VLAN_ISOLATION_SRC_INNER = 0,
    VLAN_ISOLATION_SRC_OUTER,
    VLAN_ISOLATION_SRC_FORWARD,
    VLAN_ISOLATION_SRC_END
} rtk_port_vlanIsolationSrc_t;

typedef struct rtk_port_vlanIsolationEntry_e
{
    rtk_enable_t enable;
    rtk_vlan_t vid; /* VID low bound for 8390,9300,9310 */
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    rtk_vlan_t vid_high;
#endif
    rtk_portmask_t portmask;
} rtk_port_vlanIsolationEntry_t;

typedef enum rtk_port_10gMeida_e
{
    PORT_10GMEDIA_NONE,
    PORT_10GMEDIA_FIBER_10G,
    PORT_10GMEDIA_FIBER_1G,
    PORT_10GMEDIA_DAC_50CM,
    PORT_10GMEDIA_DAC_100CM,
    PORT_10GMEDIA_DAC_300CM,
    PORT_10GMEDIA_DAC_500CM,
    PORT_10GMEDIA_DAC_END,
    PORT_10GMEDIA_END
} rtk_port_10gMedia_t;

typedef enum phy_polarity_e
{
    PHY_POLARITY_NORMAL = 0,
    PHY_POLARITY_INVERSE = 1,
    PHY_POLARITY_END
} rtk_phy_polarity_t;

typedef struct rtk_port_phyPolarCtrl_s
{
    rtk_phy_polarity_t      phy_polar_tx;
    rtk_phy_polarity_t      phy_polar_rx;
} rtk_port_phyPolarCtrl_t;


typedef enum rtk_port_phyTestModeChannel_s
{
    PHY_TESTMODE_CHANNEL_NONE          = 0,
    PHY_TESTMODE_CHANNEL_A,
    PHY_TESTMODE_CHANNEL_B,
    PHY_TESTMODE_CHANNEL_C,
    PHY_TESTMODE_CHANNEL_D,
    PHY_TESTMODE_CHANNEL_END
} rtk_port_phyTestModeChannel_t;

#define RTK_PHY_LED_MODE_MDI_DISABLE                            0xF

#define RTK_PHY_LED_IND_STATUS_SEL_1000M_SPEED                  (0x1 << 0)  /* LED light when speed is at 1000M */
#define RTK_PHY_LED_IND_STATUS_SEL_100M_SPEED                   (0x1 << 1)  /* LED light when speed is at 100M */
#define RTK_PHY_LED_IND_STATUS_SEL_10M_SPEED                    (0x1 << 2)  /* LED light when speed is at 10M */
#define RTK_PHY_LED_IND_STATUS_SEL_1000M_ACTIVITY               (0x1 << 3)  /* LED blink when port is tx or tx */
#define RTK_PHY_LED_IND_STATUS_SEL_100M_ACTIVITY                (0x1 << 4)  /* LED blink when port is tx or tx */
#define RTK_PHY_LED_IND_STATUS_SEL_10M_ACTIVITY                 (0x1 << 5)  /* LED blink when port is tx or tx */
#define RTK_PHY_LED_IND_STATUS_SEL_DUPLEX                       (0x1 << 6)  /* LED light when full-duplex */
#define RTK_PHY_LED_IND_STATUS_SEL_COLLISION                    (0x1 << 7)  /* LED blink when collision */
#define RTK_PHY_LED_IND_STATUS_SEL_TX_ACTIVITY                  (0x1 << 8)  /* LED blink when port is tx */
#define RTK_PHY_LED_IND_STATUS_SEL_RX_ACTIVITY                  (0x1 << 9)  /* LED blink when port is rx */

typedef struct rtk_phy_ledMode_s
{
    uint16  led_id;                 /* led ID */
    uint16  mdi;                    /* MDI number or RTK_PHY_LED_MODE_MDI_DISABLE */
    uint32  led_ind_status_sel;     /* LED indicator status select */
} rtk_phy_ledMode_t;


typedef enum rtk_phy_ledBlinkRate_e
{
    RTK_PHY_LED_CTRL_BLINK_RATE_32MS = 0,
    RTK_PHY_LED_CTRL_BLINK_RATE_48MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_64MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_96MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_128MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_256MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_512MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_1024MS,
    RTK_PHY_LED_CTRL_BLINK_RATE_END
} rtk_phy_ledBlinkRate_t;


typedef enum rtk_phy_ledBurstCycle_e
{
    RTK_PHY_LED_CTRL_BURST_CYCLE_8MS = 0,
    RTK_PHY_LED_CTRL_BURST_CYCLE_16MS,
    RTK_PHY_LED_CTRL_BURST_CYCLE_32MS,
    RTK_PHY_LED_CTRL_BURST_CYCLE_64MS,
    RTK_PHY_LED_CTRL_BURST_CYCLE_END
} rtk_phy_ledBurstCycle_t;

typedef enum rtk_phy_ledClockCycle_e
{
    RTK_PHY_LED_CTRL_CLOCK_CYCLE_32NS = 0,
    RTK_PHY_LED_CTRL_CLOCK_CYCLE_64NS,
    RTK_PHY_LED_CTRL_CLOCK_CYCLE_96NS,
    RTK_PHY_LED_CTRL_CLOCK_CYCLE_192NS,
    RTK_PHY_LED_CTRL_CLOCK_CYCLE_END
} rtk_phy_ledClockCycle_t;

typedef enum rtk_phy_ledActiveLow_e
{
    RTK_PHY_LED_CTRL_ACTIVE_HIGH = 0,
    RTK_PHY_LED_CTRL_ACTIVE_LOW,
    RTK_PHY_LED_CTRL_ACTIVE_END
} rtk_phy_ledActiveLow_t;

typedef struct rtk_phy_ledCtrl_s
{
    rtk_phy_ledBlinkRate_t   blink_rate;
    rtk_phy_ledBurstCycle_t  burst_cycle;
    rtk_phy_ledClockCycle_t  clock_cycle;
    rtk_phy_ledActiveLow_t   active_low;
} rtk_phy_ledCtrl_t;


#define RTK_PORT_PHY_1G_TEST_MODE1                  0x1
#define RTK_PORT_PHY_1G_TEST_MODE2                  0x2
#define RTK_PORT_PHY_1G_TEST_MODE3                  0x3
#define RTK_PORT_PHY_1G_TEST_MODE4                  0x4
#define RTK_PORT_PHY_10G_TEST_MODE_NONE             0x1000
#define RTK_PORT_PHY_10G_TEST_MODE1                 0x1010
#define RTK_PORT_PHY_10G_TEST_MODE2                 0x1020
#define RTK_PORT_PHY_10G_TEST_MODE3                 0x1030
#define RTK_PORT_PHY_10G_TEST_MODE4_1               0x1041  /* Test Mode 4 with Dual Tone 1 (Transmitter_test_frequencies=1) */
#define RTK_PORT_PHY_10G_TEST_MODE4_2               0x1042  /* Test Mode 4 with Dual Tone 2 (Transmitter_test_frequencies=2) */
#define RTK_PORT_PHY_10G_TEST_MODE4_4               0x1044  /* Test Mode 4 with Dual Tone 3 (Transmitter_test_frequencies=4) */
#define RTK_PORT_PHY_10G_TEST_MODE4_5               0x1045  /* Test Mode 4 with Dual Tone 4 (Transmitter_test_frequencies=5) */
#define RTK_PORT_PHY_10G_TEST_MODE4_6               0x1046  /* Test Mode 4 with Dual Tone 5 (Transmitter_test_frequencies=6) */
#define RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE1       RTK_PORT_PHY_10G_TEST_MODE4_1
#define RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE2       RTK_PORT_PHY_10G_TEST_MODE4_2
#define RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE3       RTK_PORT_PHY_10G_TEST_MODE4_4
#define RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE4       RTK_PORT_PHY_10G_TEST_MODE4_5
#define RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE5       RTK_PORT_PHY_10G_TEST_MODE4_6
#define RTK_PORT_PHY_10G_TEST_MODE5                 0x1050
#define RTK_PORT_PHY_10G_TEST_MODE6                 0x1060
#define RTK_PORT_PHY_10G_TEST_MODE7                 0x1070
#define RTK_PORT_PHY_100M_TEST_MODE_NONE            0xFE00
#define RTK_PORT_PHY_100M_TEST_MODE_IEEE            0xFE01  /* 100BASE-TX IEEE test mode */
#define RTK_PORT_PHY_100M_TEST_MODE_ANSI_JITTER     0xFE02  /* 100BASE-TX ANSI Jitter test mode */
#define RTK_PORT_PHY_100M_TEST_MODE_ANSI_DROOP      0xFE03  /* 100BASE-TX ANSI Droop test mode */


#define RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS                     0x00000001
typedef struct rtk_port_phyTestMode_s
{
    int32                           mode;
    rtk_port_phyTestModeChannel_t   channel;
    uint32                          flags;
} rtk_port_phyTestMode_t;


typedef struct rtk_port_swMacPollPhyStatus_s
{
    uint32              reslStatus; /* resolution status */
    rtk_port_media_t    media; /* PORT_MEDIA_COPPER or PORT_MEDIA_FIBER */
} rtk_port_swMacPollPhyStatus_t;

typedef struct rtk_sdsCfg_s
{
    rtk_serdesMode_t    sdsMode;
} rtk_sdsCfg_t;

typedef enum rtk_port_phySdsRxCaliStatus_e
{
    PHY_SDS_RXCALI_STATUS_NOINFO = 0,   /* rx-calibration has not been processed */
    PHY_SDS_RXCALI_STATUS_OK,           /* rx-calibration processed and result is ok */
    PHY_SDS_RXCALI_STATUS_FAILED,       /* rx-calibration processed and result is fail */
    PHY_SDS_RXCALI_STATUS_END
} rtk_port_phySdsRxCaliStatus_t;

typedef struct rtk_phy_macIntfSdsLinkStatus_s
{
    rtk_port_linkStatus_t   link_status[RTK_PHY_MAC_INTF_SDS_MAX];      /* Link status, index is serdes id */
    int32                   sds_num;                                    /* number of SerDes link status is availible in link_status */
} rtk_phy_macIntfSdsLinkStatus_t;

typedef int32 (*rtk_port_linkMon_callback_t)(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus);
typedef int32 (*rtk_port_phyReconfig_callback_t)(int32 unit, rtk_portmask_t * portmask);


/*
 * Macro Declaration
 */
/* for backward compatiable */
#define rtk_port_rtctResult_get rtk_diag_portRtctResult_get
#define rtk_port_rtctEnable_set rtk_diag_rtctEnable_set
#define ABILITY_BIT_ON      (1)
#define ABILITY_BIT_OFF     (0)
#define RTK_PORT_FLASHIMG_SIZE (10U)
#define RTK_PORT_FLASHIMG_SIZE_MAX (1024U * 300U)

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize port module before calling any port APIs.
 * Changes:
 *      None
 */
extern int32
rtk_port_init(uint32 unit);

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      rtk_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The link status of the port is as following:
 *      - PORT_LINKDOWN
 *      - PORT_LINKUP
 * Changes:
 *      None
 */
extern int32
rtk_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

/* Function Name:
 *      rtk_port_linkMedia_get
 * Description:
 *      Get the link status with media information of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 *      pMedia  - pointer to the media type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The link status of the port is as following:
 *          - PORT_LINKDOWN
 *          - PORT_LINKUP
 *      (2) The media type of the port is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 *      (3) When the link status is link-down, the return media should be ignored.
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMedia_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia);

/* Function Name:
 *      rtk_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *          - PORT_SPEED_2G  (Applicable to 8380)
 *          - PORT_SPEED_10G (Applicable to 8390)
 *          - PORT_SPEED_500M (Applicable to 9300, 9310)
 *          - PORT_SPEED_2_5G (Applicable to 9300, 9310)
 *          - PORT_SPEED_5G (Applicable to 9300, 9310)
 *      (2) The duplex mode of the port is as following:
 *          - PORT_HALF_DUPLEX
 *          - PORT_FULL_DUPLEX
 * Changes:
 *      None
 */
extern int32
rtk_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex);

/* Function Name:
 *      rtk_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_flowctrl_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus);

/* Function Name:
 *      rtk_port_phyAutoNegoEnable_get
 * Description:
 *      Get PHY ability of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyAutoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely switch the mode without calling ability setting API again
 * Changes:
 *      None
 */
extern int32
rtk_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyAutoNegoAbility_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility);

/* Function Name:
 *      rtk_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 * Changes:
 *      None
 */
extern int32
rtk_port_phyAutoNegoAbility_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility);

/* Function Name:
 *      rtk_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      unit         - unit id
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyForceModeAbility_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl);

/* Function Name:
 *      rtk_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      speed       - port speed
 *      duplex      - port duplex mode
 *      flowControl - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      (1) You can set these abilities no matter which mode PHY currently stays on
 *      (2) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M (only for fiber media)
 *      (3) The duplex mode of the port is as following:
 *          - PORT_HALF_DUPLEX
 *          - PORT_FULL_DUPLEX
 * Changes:
 *      None
 */
extern int32
rtk_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl);

/* Function Name:
 *      rtk_port_phyForceFlowctrlMode_get
 * Description:
 *      Get the port flow control mode in the PHY force mode
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pMode     - pointer to the port flow control mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_port_phyForceFlowctrlMode_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_port_flowctrl_mode_t    *pMode);

/* Function Name:
 *      rtk_port_phyForceFlowctrlMode_set
 * Description:
 *      Set the port flow control mode in the PHY force mode.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pMode     - pointer to the port flow control mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Applicable:
 *      8380, 8390SDS, 9300, 9310SDS
 * Note:
 *      This API configure the flow control mode to a software shadow database,
 *      it shall use together with rtk_port_phyForceModeAbility_set() to apply the config to the chip.
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_port_phyForceFlowctrlMode_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_port_flowctrl_mode_t    *pMode);

/* Function Name:
 *      rtk_port_phyMasterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 * Changes:
 *      None
 */
extern int32
rtk_port_phyMasterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual);

/* Function Name:
 *      rtk_port_phyMasterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      masterSlave - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      This function only works on giga/ 10g port to set its master/slave mode configuration.
 * Changes:
 *      None
 */
extern int32
rtk_port_phyMasterSlave_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_masterSlave_t  masterSlave);

/* Function Name:
 *      rtk_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      page  - page id
 *      reg   - reg id
 * Output:
 *      pData - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData);

/* Function Name:
 *      rtk_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      page - page id
 *      reg  - reg id
 *      data - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data);

/* Function Name:
 *      rtk_port_phyExtParkPageReg_get
 * Description:
 *      Get PHY register data of the specific port with extension page and parking page parameters
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      mainPage - main page id
 *      extPage  - extension page id
 *      parkPage - parking page id
 *      reg      - reg id
 * Output:
 *      pData    - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyExtParkPageReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              *pData);

/* Function Name:
 *      rtk_port_phyExtParkPageReg_set
 * Description:
 *      Set PHY register data of the specific port with extension page and parking page parameters
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      mainPage - main page id
 *      extPage  - extension page id
 *      parkPage - parking page id
 *      reg      - reg id
 *      data     - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyExtParkPageReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              data);

/* Function Name:
 *      rtk_port_phymaskExtParkPageReg_set
 * Description:
 *      Set PHY register data of the specific portmask with extension page and parking page parameters
 * Input:
 *      unit      - unit id
 *      pPortmask - pointer to the portmask
 *      mainPage  - main page id
 *      extPage   - extension page id
 *      parkPage  - parking page id
 *      reg       - reg id
 *      data      - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phymaskExtParkPageReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              data);

/* Function Name:
 *      rtk_port_phyMmdReg_get
 * Description:
 *      Get PHY MMD register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyMmdReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              *pData);

/* Function Name:
 *      rtk_port_phyMmdReg_set
 * Description:
 *      Set PHY MMD register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyMmdReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data);

/* Function Name:
 *      rtk_port_phymaskMmdReg_set
 * Description:
 *      Set PHY MMD register data of the specific portmask
 * Input:
 *      unit      - unit id
 *      pPortmask - pointer to the portmask
 *      mmdAddr   - mmd device address
 *      mmdReg    - mmd reg id
 *      data      - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phymaskMmdReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data);

/* Function Name:
 *      rtk_port_cpuPortId_get
 * Description:
 *      Get CPU port id of the specific unit
 * Input:
 *      unit  - unit id
 * Output:
 *      pPort - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort);

/* Function Name:
 *      rtk_port_isolation_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each port can specify an allowable egress forwarding portmask.
 *      (2) To config switches in stacking system, uses rtk_port_isolationExt_get() instead.
 * Changes:
 *      None
 */
extern int32
rtk_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_port_isolation_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pPortmask   - pointer to the portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID   - invalid unit id
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each port can specify an allowable egress forwarding portmask.
 *      (2) To config switches in stacking system, uses rtk_port_isolationExt_set() instead.
 * Changes:
 *      None
 */
extern int32
rtk_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_port_isolationExt_get
 * Description:
 *      Get the port isolation portmask for packet received from specified unit and port.
 * Input:
 *      unit        - unit id
 *      devID       - source dev id
 *      srcPort     - source port id
 * Output:
 *      pPortmask   - pointer to the portmask that packet can be forwarded to when it is received from devID and srcPort
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking system, each (dev, port) pair should be assigned the permit outgoing portmask of this device.
 * Changes:
 *      [3.2.0]
 *          Change srcUnit to devID.
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_isolationExt_get(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_port_isolationExt_set
 * Description:
 *      Set the port isolation portmask for packet received from specified unit and port.
 * Input:
 *      unit        - unit id
 *      devID       - source dev id
 *      srcPort     - source port id
 *      pPortmask   - pointer to the portmask that packet can be forwarded to when it is received from devID and srcPort
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID   - invalid unit id
 *      RT_ERR_NOT_INIT  - The module is not initial
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking system, each (dev, port) pair should be assigned the permit outgoing portmask of this device.
 * Changes:
 *      [3.2.0]
 *          Change srcUnit to devID.
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_isolationExt_set(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_port_isolation_add
 * Description:
 *      Add an isolation port to the certain port
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      iso_port - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Default value of each port represent bit in portmask is 1
 *      (2) Port and iso_port will be isolated when this API is called
 *      (3) The iso_port to the relative portmask bit will be set to 1
 * Changes:
 *      None
 */
extern int32
rtk_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port);

/* Function Name:
 *      rtk_port_isolation_del
 * Description:
 *      Delete an existing isolation port of the certain port
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      iso_port - isolation port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Default value of each port represent bit in portmask is 1
 *      (2) Isolated status between the port and the iso_port is removed when this API is called
 *      (3) The iso_port to the relative portmask bit will be set to 0
 * Changes:
 *      None
 */
extern int32
rtk_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port);

/* Function Name:
 *      rtk_port_isolationRestrictRoute_get
 * Description:
 *      Get status of routed packet whether restrict by port isolation
 * Input:
 *      unit     - unit id
 * Output:
 *      pEnable - pointer to enable status of port isolation restrict route
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_isolationRestrictRoute_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_isolationRestrictRoute_set
 * Description:
 *      Set status of routed packet whether restrict by port isolation
 * Input:
 *      unit     - unit id
 *      enable - enable status of port isolaiton restrict route
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_isolationRestrictRoute_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyComboPortMedia_get
 * Description:
 *      Get PHY port media of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer to the port media
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 9300SDS, 8390SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyComboPortMedia_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

/* Function Name:
 *      rtk_port_phyComboPortMedia_set
 * Description:
 *      Set PHY port media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8208D, 8208L, 8214FB, 8214B, 8212B, 8218FB, 8214FC
 * Note:
 *      (1) The media value is as following:
 *          - PORT_MEDIA_COPPER
 *          - PORT_MEDIA_FIBER
 * Changes:
 *      None
 */
extern int32
rtk_port_phyComboPortMedia_set(uint32 unit, rtk_port_t port, rtk_port_media_t media);

/* Function Name:
 *      rtk_port_backpressureEnable_get
 * Description:
 *      Get the half duplex backpressure enable status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of backpressure in half duplex mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac back pressure enable status of the port is as following:
 *          - DISABLED
 *          - ENABLED
 *      (2) Used to support backpressure in half mode.
 * Changes:
 *      None
 */
extern int32
rtk_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_backpressureEnable_set
 * Description:
 *      Set the half duplex backpressure enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of backpressure in half duplex mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac back pressure enable status of the port is as following:
 *          - DISABLED
 *          - ENABLED
 *      (2) Used to support backpressure in half mode.
 * Changes:
 *      None
 */
extern int32
rtk_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_adminEnable_set
 * Description:
 *      Set port admin configuration of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - port admin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_linkMon_enable
 * Description:
 *      Enable link monitor thread
 * Input:
 *      scan_interval_us - scan interval in us.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - scan interval is too small
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When enable link monitor thread, all link change interrupt will be handled by thread.
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMon_enable(uint32 unit, uint32 scan_interval_us);

/* Function Name:
 *      rtk_port_linkMon_disable
 * Description:
 *      Disable link scan thread
 * Input:
 *      unit                   - unit index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      When disable link monitor thread, all link change interrupt will be callback to upper layer.
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMon_disable(uint32 unit);

/* Function Name:
 *      rtk_port_linkMon_register
 * Description:
 *      Register callback function for link change notification
 * Input:
 *      linkMon_callback - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMon_register(rtk_port_linkMon_callback_t linkMon_callback);

/* Function Name:
 *      rtk_port_linkMon_unregister
 * Description:
 *      Unregister callback function for link change notification
 * Input:
 *      linkMon_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMon_unregister(rtk_port_linkMon_callback_t linkMon_callback);

/* Function Name:
 *      rtk_port_linkMon_swScanPorts_set
 * Description:
 *      Configure portmask of software linkscan for certain unit
 * Input:
 *      unit             - callback function for link change
 *      pSwScan_portmask - portmask for software scan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMon_swScanPorts_set(uint32 unit, rtk_portmask_t *pSwScan_portmask);

/* Function Name:
 *      rtk_port_linkMon_swScanPorts_get
 * Description:
 *      Get portmask of software linkscan for certain unit
 * Input:
 *      unit             - callback function for link change
 * Output:
 *      pSwScan_portmask - portmask for software scan
 * Return:
 *      RT_ERR_OK           - initialize success
 *      RT_ERR_FAILED       - initialize fail
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_linkMon_swScanPorts_get(uint32 unit, rtk_portmask_t *pSwScan_portmask);

/* Function Name:
 *      rtk_port_txEnable_get
 * Description:
 *      Get the TX enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to the port TX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_txEnable_set
 * Description:
 *      Set the TX enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of TX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The TX enable status of the port is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_rxEnable_get
 * Description:
 *      Get the RX enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to the port RX status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_rxEnable_set
 * Description:
 *      Set the RX enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The RX enable status of the port is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_specialCongest_get
 * Description:
 *      Get the congest seconds of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pSecond - pointer to congest timer (seconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If a port is continuously congested for congest seconds, the system would drain out the packets of the port,
 *      so the control traffic has chance to transmit to solve the congest situation.
 * Changes:
 *      None
 */
extern int32
rtk_port_specialCongest_get(uint32 unit, rtk_port_t port, uint32* pSecond);

/* Function Name:
 *      rtk_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If a port is continuously congested for congest seconds, the system would drain out the packets of the port,
 *      so the control traffic has chance to transmit to solve the congest situation.
 * Changes:
 *      None
 */
extern int32
rtk_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second);

/* Function Name:
 *      rtk_port_greenEnable_get
 * Description:
 *      Get the status of green feature of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_greenEnable_set
 * Description:
 *      Set the statue of green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8208G, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      Needs to restart auto-negotiation for the configuration to take effect.
 * Changes:
 *      None
 */
extern int32
rtk_port_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyCrossOverMode_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      The valid value of mode is as following:
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 * Changes:
 *      None
 */
extern int32
rtk_port_phyCrossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode);

/* Function Name:
 *      rtk_port_phyCrossOverMode_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      The valid value of mode is as following:
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 * Changes:
 *      None
 */
extern int32
rtk_port_phyCrossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode);

/* Function Name:
 *      rtk_port_phyCrossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pStatus - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Applicable:
 *      8214C, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      The valid value of mode is as following:
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 * Changes:
 *      None
 */
extern int32
rtk_port_phyCrossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);


/* Function Name:
 *      rtk_port_flowCtrlEnable_get
 * Description:
 *      Get the flow control status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the status of the flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API get the flow control status by port based, no matter N-WAY is enabled or disabled.
 * Changes:
 *      None
 */
extern int32
rtk_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_flowCtrlEnable_set
 * Description:
 *      Set the flow control status to the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API is apply the flow control status by port based, no matter N-WAY is enabled or disabled.
 * Changes:
 *      None
 */
extern int32
rtk_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyComboPortFiberMedia_get
 * Description:
 *      Get PHY port fiber media of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer to the port fiber media
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8214FB, 8214B, 8212B, 8218FB, 8214FC
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_100
 *      - PORT_FIBER_MEDIA_AUTO
 * Changes:
 *      None
 */
extern int32
rtk_port_phyComboPortFiberMedia_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia);

/* Function Name:
 *      rtk_port_phyComboPortFiberMedia_set
 * Description:
 *      Set PHY port fiber media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port fiber media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Applicable:
 *      8214FB, 8214B, 8212B, 8218FB, 8214FC
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_100
 *      - PORT_FIBER_MEDIA_AUTO
 * Changes:
 *      None
 */
extern int32
rtk_port_phyComboPortFiberMedia_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media);

/* Function Name:
 *      rtk_port_linkDownPowerSavingEnable_get
 * Description:
 *      Get the statue of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_linkDownPowerSavingEnable_set
 * Description:
 *      Set the statue of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_vlanBasedIsolationEntry_get
 * Description:
 *      Get VLAN-based port isolation entry
 * Input:
 *      unit   - unit id
 *      index  - index id
 * Output:
 *      pEntry - pointer to vlan-based port isolation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Valid index is 0~15.
 * Changes:
 *      None
 */
extern int32
rtk_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry);

/* Function Name:
 *      rtk_port_vlanBasedIsolationEntry_set
 * Description:
 *      Set VLAN-based port isolation entry
 * Input:
 *      unit   - unit id
 *      index  - index id
 *      pEntry - pointer to vlan-based port isolation entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                              - invalid unit id
 *      RT_ERR_NOT_INIT                             - The module is not initial
 *      RT_ERR_INPUT                                - invalid input parameter
 *      RT_ERR_NULL_POINTER                         - input parameter may be null pointer
 *      RT_ERR_VLAN_VID                             - invalid vid
 *      RT_ERR_PORT_VLAN_ISO_VID_EXIST_IN_OTHER_IDX - vid exists in other entry
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Valid index is 0~15.
 * Changes:
 *      None
 */
extern int32
rtk_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry);

/* Function Name:
 *      rtk_port_vlanBasedIsolation_vlanSource_get
 * Description:
 *      Get VLAN source of VLAN-based port isolation
 * Input:
 *      unit     - unit id
 * Output:
 *      pVlanSrc - pointer to vlan source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Always use forward vid for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_port_vlanBasedIsolation_vlanSource_get(uint32 unit, rtk_port_vlanIsolationSrc_t *pVlanSrc);

/* Function Name:
 *      rtk_port_vlanBasedIsolation_vlanSource_set
 * Description:
 *      Set VLAN source of VLAN-based port isolation
 * Input:
 *      unit    - unit id
 *      vlanSrc - vlan source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      Always use forward vid for 9300,9310
 * Changes:
 *      None
 */
extern int32
rtk_port_vlanBasedIsolation_vlanSource_set(uint32 unit, rtk_port_vlanIsolationSrc_t vlanSrc);

/* Function Name:
 *      rtk_port_vlanBasedIsolationEgrBypass_get
 * Description:
 *      Get Egress Port Bypass Status  of VLAN-based port isolation
 * Input:
 *      unit     - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to egress port bypass status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_vlanBasedIsolationEgrBypass_get(uint32 unit, rtk_port_t port,rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_vlanBasedIsolationEgrBypass_set
 * Description:
 *      Set egress port bypass status of VLAN-based port isolation
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pEnable - pointer to egress port bypass status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_vlanBasedIsolationEgrBypass_set(uint32 unit, rtk_port_t port,rtk_enable_t enable);
/* Function Name:
 *      rtk_port_gigaLiteEnable_get
 * Description:
 *      Get the statue of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8214C, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_gigaLiteEnable_set
 * Description:
 *      Set the statue of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      8214C, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_2pt5gLiteEnable_get
 * Description:
 *      Get the statue of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of 2.5G Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8226, 8284
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
extern int32
rtk_port_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_2pt5gLiteEnable_set
 * Description:
 *      Set the statue of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of 2.5G Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      8226, 8284
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
extern int32
rtk_port_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyReconfig_register
 * Description:
 *      Register callback function for PHY need to reconfigure notification
 * Input:
 *       unit                                - unit index
 *       phyNotification_callback    - callback function for reconfigure notification
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyReconfig_register(uint32 unit, rtk_port_phyReconfig_callback_t phyNotification_callback);

/* Function Name:
 *      rtk_port_phyReconfig_unregister
 * Description:
 *      UnRegister callback function for PHY need to reconfigure notification
 * Input:
 *	  unit					 - unit index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyReconfig_unregister(uint32 unit);

/* Function Name:
 *      rtk_port_downSpeedEnable_get
 * Description:
 *      Get down speed status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - down speed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_downSpeedEnable_set
 * Description:
 *      Set down speed status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - down speed status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_port_fiberDownSpeedEnable_get
 * Description:
 *      Get fiber down speed status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber down speed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8218FB, 8214FC, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_fiberDownSpeedEnable_set
 * Description:
 *      Set fiber down speed status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber down speed status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8218FB, 8214FC, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_port_fiberNwayForceLinkEnable_get
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Nway force links status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8218FB, 8214FC, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberNwayForceLinkEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_fiberNwayForceLinkEnable_set
 * Description:
 *      When fiber port is configured N-way,
 *      which can link with link partner is configured force mode.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Nway force links status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8218FB, 8214FC, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberNwayForceLinkEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_port_fiberUnidirEnable_get
 * Description:
 *      Get fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable       - - pointer to the enable status of fiber unidirection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_port_fiberUnidirEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of fiber unidirection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_port_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_fiberOAMLoopBackEnable_set
 * Description:
 *     Set Fiber-port OAM Loopback enable or not,
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber oam loopback enable or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8380, 8390
 * Note:
 *      9300,931X is supported by new method, API is not avaliable currently.
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyLoopBackEnable_get
 * Description:
 *      Get PHY Loopback featrue
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pEnable - ENABLED: Enable loopback;
 *               DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8214C, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyLoopBackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_phyLoopBackEnable_set
 * Description:
 *      Set PHY Loopback featrue
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - ENABLED: Enable loopback;
 *               DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8214C, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8226
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_10gMedia_set
 * Description:
 *      Set 10G port media of the specific port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - port media
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not support the function
 *      RT_ERR_TIMEOUT  - exec function time out
 * Applicable:
 *      8295R, 8390SDS, 9300SDS, 9310SDS
 * Note:
 *      (1) The media value is as following:
 *          - PORT_10GMEDIA_FIBER_10G,
 *          - PORT_10GMEDIA_FIBER_1G,
 *          - PORT_10GMEDIA_DAC_50CM,
 *          - PORT_10GMEDIA_DAC_100CM,
 *          - PORT_10GMEDIA_DAC_300CM,
 * Changes:
 *      None
 */
extern int32
rtk_port_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

/* Function Name:
 *      rtk_port_10gMedia_get
 * Description:
 *      Get 10G port media of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      media   - pointer to the media type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8295R, 8390SDS, 9300SDS, 9310SDS
 * Note:
 *      (1) The media type of the port is as following:
 *          - PORT_10GMEDIA_FIBER_10G,
 *          - PORT_10GMEDIA_FIBER_1G,
 *          - PORT_10GMEDIA_DAC_50CM,
 *          - PORT_10GMEDIA_DAC_100CM,
 *          - PORT_10GMEDIA_DAC_300CM,
 * Changes:
 *      None
 */
extern int32
rtk_port_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *media);

/* Function Name:
 *      rtk_port_phyFiberTxDis_set
 * Description:
 *      Set PHY fiber Tx disable signal
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - ENABLED: Enable Tx disable signal;
 *                DISABLED: Disable Tx disable signal.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8218FB, 8214FC, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyFiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyFiberTxDisPin_set
 * Description:
 *      Set PHY fiber Tx disable signal GPO output
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      data      - GPO pin value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8218FB, 8214FC, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyFiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data);

/* Function Name:
 *      rtk_port_fiberRxEnable_get
 * Description:
 *      Get fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - fiber Rx enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8214FC, 8218FB
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberRxEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_fiberRxEnable_set
 * Description:
 *      Set fiber Rx enable status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - fiber Rx enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8214FC, 8218FB
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_fiberRxEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyIeeeTestMode_set
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
 * Applicable:
 *      8218B, 8214FC, 8218FB, 8214FB, 8214B, 8212B, 8218D, 8226
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_phyIeeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);


/* Function Name:
 *      rtk_port_phyPolar_get
 * Description:
 *      Get 10GE fiber port polarity configure
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pPolarCtrl - polarity configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8295R, 9300SDS
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_phyPolar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl);

/* Function Name:
 *      rtk_port_phyPolar_set
 * Description:
 *      Configure 10GE fiber port polarity
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      polarCtrl - polarity configuration
 * Output:
  *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8295R, 9300SDS
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_port_phyPolar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl);

/* Function Name:
 *      rtk_port_phyEyeMonitor_start
 * Description:
 *      Trigger eye monitor function
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes id or 0 for serdes port or the PHY has no serdes id.
 *      frameNum- frame number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      8295R, 8214QF, 9300SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_port_phyEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum);

/* Function Name:
 *      rtk_port_imageFlash_load
 * Description:
 *      load PHY config image into flash
 * Input:
 *      unit - unit id
 *      port - port id
 *      size - image size
 *      image - image
 * Output:
  *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      cust_phy1
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_port_imageFlash_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image);

/* Function Name:
 *      rtk_port_phySds_get
 * Description:
 *      Get PHY SerDes
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_port_phySds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      rtk_port_phySds_set
 * Description:
 *      Configure PHY SerDes
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_port_phySds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      rtk_port_phySdsRxCaliStatus_get
 * Description:
 *      Get PHY SerDes rx-calibration status
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsId   - serdes ID
 * Output:
 *      rtk_port_phySdsRxCaliStatus_t   - rx-calibration status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_OUT_OF_RANGE - invalid serdes id
 * Applicable:
 *      8295R
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_port_phySdsRxCaliStatus_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus);

/* Function Name:
 *      rtk_port_phyReset_set
 * Description:
 *      Set PHY standard register Reset bit (0.15).
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED  : Set Reset bit; Reset PHY;
 *                            DISABLED : Clear Reset bit; PHY back to normal operation..
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
extern int32
rtk_port_phyReset_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      rtk_port_phyLinkStatus_get
 * Description:
 *      Get PHY link status from standard register (1.2).
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218D
 * Note:
 *      The Link Status bit (Status Register 1.2) has LL (Latching Low) attribute
 *      for link failure. Please refer IEEE 802.3 for detailed.
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
extern int32
rtk_port_phyLinkStatus_get(uint32 unit, rtk_port_t port,
    rtk_port_linkStatus_t *pStatus);

/* Function Name:
 *      rtk_port_phyPeerAutoNegoAbility_get
 * Description:
 *      Get ability from link partner advertisement auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
extern int32
rtk_port_phyPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility);

/* Function Name:
 *      rtk_port_phyMacIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side interface serdes mode
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pSerdesMode  - PHY serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
extern int32
rtk_port_phyMacIntfSerdesMode_get(uint32 unit, rtk_port_t port,
    rt_serdesMode_t *pSerdesMode);

/* Function Name:
 *      rtk_port_phyLedMode_set
 * Description:
 *      Configure LED mode to PHY. PHY will send out data to LED panel for port status indication
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pLedMode - LED mode select.
 *                 led_id is used to specify which LED to config.
 *                 mdi is PHY port ID of each PHY (starting from 0). It config the LED to display which port's status.
 *                 led_ind_status_sel can select multiple indicators by OR with mutiple RTK_PHY_LED_IND_STATUS_SEL_xxx macros.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *         New added function.
 */
extern int32
rtk_port_phyLedMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode);

/* Function Name:
 *      rtk_port_phyLedCtrl_get
 * Description:
 *      Get configuration of LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pLedCtrl - LED control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *         New added function.
 */
extern int32
rtk_port_phyLedCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

/* Function Name:
 *      rtk_port_phyLedCtrl_set
 * Description:
 *      Configure LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pLedCtrl - LED control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *         New added function.
 */
extern int32
rtk_port_phyLedCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

/* Function Name:
 *      rtk_port_phyMacIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Applicable:
 *      8218D, 8226
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.2]
 *         New added function.
 */
extern int32
rtk_port_phyMacIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus);


/* Function Name:
 *      rtk_port_phySdsEyeParam_get
 * Description:
 *      Get SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pEyeParam - eye parameter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.5.0]
 *          New added function.
 */
extern int32
rtk_port_phySdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

/* Function Name:
 *      rtk_port_phySdsEyeParam_set
 * Description:
 *      Set SerDes eye parameters
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pEyeParam - eye parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.5.0]
 *          New added function.
 */
extern int32
rtk_port_phySdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

/* Function Name:
 *      rtk_port_phyMdiLoopbackEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_port_phyMdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_phyMdiLoopbackEnable_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_port_phyMdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


#endif /* __RTK_PORT_H__ */

