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
 * $Revision: 104195 $
 * $Date: 2020-02-20 15:55:55 +0800 (Thu, 20 Feb 2020) $
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 *           (2) UDLD
 *           (3) RLDP
 */

#ifndef __RTK_PHY_H__
#define __RTK_PHY_H__

#include <common/rt_type.h>
#include <hal/chipdef/chipdef.h>
#include <rtk/sds.h>
#include <rtk/time.h>

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

typedef enum rtk_port_media_chg_e
{
    PORT_DEFAULT = 0,
    PORT_MEDIA_CHG,
    PORT_MEDIA_CHG_END
} rtk_port_media_chg_t;

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
    PORT_SPEED_2_5G_LITE,
    PORT_SPEED_5G_LITE,
    PORT_SPEED_10G_LITE,
    PORT_SPEED_END
} rtk_port_speed_t;

typedef enum rtk_port_lite_mode_e
{
    PORT_LITE_1G,
    PORT_LITE_2P5G,
    PORT_LITE_5G,
    PORT_LITE_10G,
    PORT_LITE_END
} rtk_port_lite_mode_t;

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
    PORT_10GMEDIA_FIBER_2_5G,
    PORT_10GMEDIA_FIBER_100M,
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

typedef enum rtk_port_phy_dbg_cnt_e
{
    PHY_DBG_CNT_RX,
    PHY_DBG_CNT_RX_ERR,
    PHY_DBG_CNT_RX_CRCERR,
    PHY_DBG_CNT_LDPC_ERR,
    PHY_DBG_CNT_END
} rtk_port_phy_dbg_cnt_t;

#define RTK_PHY_LED_MODE_MDI_DISABLE                            0xF

#define RTK_PHY_LED_IND_STATUS_SEL_1000M_SPEED                  (0x1 << 0)  /* LED light when speed is at 1000M */
#define RTK_PHY_LED_IND_STATUS_SEL_100M_SPEED                   (0x1 << 1)  /* LED light when speed is at 100M */
#define RTK_PHY_LED_IND_STATUS_SEL_10M_SPEED                    (0x1 << 2)  /* LED light when speed is at 10M */
#define RTK_PHY_LED_IND_STATUS_SEL_1000M_ACTIVITY               (0x1 << 3)  /* LED blink when port is tx or rx */
#define RTK_PHY_LED_IND_STATUS_SEL_100M_ACTIVITY                (0x1 << 4)  /* LED blink when port is tx or rx */
#define RTK_PHY_LED_IND_STATUS_SEL_10M_ACTIVITY                 (0x1 << 5)  /* LED blink when port is tx or rx */
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

#define RTK_PORT_PHY_1G_TEST_MODE_NONE              0x0
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

/* PHY Interrupt Type */
typedef enum rtk_phy_intr_e
{
    RTK_PHY_INTR_COMMON,                        /* Normal interrupt */
    RTK_PHY_INTR_RLFD,                          /* Rapid Link Fault Detection interrupt for Fast Link Down Function */
    RTK_PHY_INTR_TM_LOW,                        /* thermal meter over temperature interrupt for level low */
    RTK_PHY_INTR_TM_HIGH,                       /* thermal meter over temperature interrupt for level High */
    RTK_PHY_INTR_MACSEC,                        /* MACSEC */
    RTK_PHY_INTR_PTP1588,                       /* PTP 1588 interrupt */
    RTK_PHY_INTR_END
} rtk_phy_intr_t;

/* PHY Interrupt Status Type */
typedef enum rtk_phy_intr_status_e
{
    RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV,  /* An Nway Next Page is received interrupt */
    RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE,          /* Auto-Negotiation Completed interrupt */
    RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE,          /* Link Status Changed interrupt */
    RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE,   /* ALDPS State Changes interrupt */
    RTK_PHY_INTR_STATUS_RLFD,                        /* Rapid Link Fault Detection interrupt for Fast Link Down Function */
    RTK_PHY_INTR_STATUS_TM_LOW,                      /* thermal meter over temperature interrupt for level low */
    RTK_PHY_INTR_STATUS_TM_HIGH,                     /* thermal meter over temperature interrupt for level High */
    RTK_PHY_INTR_STATUS_FATAL_ERROR,                 /* PHY fatal error interrupt */
    RTK_PHY_INTR_STATUS_MACSEC,                      /* MACSec interrupt */
    RTK_PHY_INTR_STATUS_PTP1588,                     /* PTP 1588 interrupt */
    RTK_PHY_INTR_STATUS_END
} rtk_phy_intr_status_t;

/* PHY Interrupt Status Value Type */
typedef enum rtk_phy_intr_status_type_e
{
    /* value is a bitmap with multi-type of status for one port, bit offset is defined in rtk_phy_intr_status_t*/
    RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP,
    /* value is a bitmap with one type status for multi-port in one PHY chip, bit 0 is base-port, bit 1 is base-port+1, and so on */
    RTK_PHY_INTR_STATUS_TYPE_PORT_BITMAP,
    /* value is an array of bitmap with multi-type of status for ports in one phy, bit offset is defined in rtk_phy_intr_status_t */
    RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP_PORT_ARRAY,
    RTK_PHY_INTR_STATUS_TYPE_END
} rtk_phy_intr_status_type_t;


/* PHY Interrupt Status structure for get interrupt status */
typedef struct rtk_phy_intrStatusVal_s
{
    rtk_phy_intr_status_type_t statusType;
    uint32 statusValue;                             /* for RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP, RTK_PHY_INTR_STATUS_TYPE_PORT_BITMAP */
    uint32 statusValueArray[RTK_MAX_PORT_PER_UNIT]; /* for RTK_PHY_INTR_STATUS_TYPE_STATUS_BITMAP_PORT_ARRAY */
} rtk_phy_intrStatusVal_t;

typedef int32 (*rtk_port_linkMon_callback_t)(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus);
typedef int32 (*rtk_port_phyReconfig_callback_t)(int32 unit, rtk_portmask_t * portmask);

typedef enum rtk_phy_ctrl_e {
    RTK_PHY_CTRL_AN_COMPLETE,                    /* status of auto-negotiation complete, R */
    RTK_PHY_CTRL_LED_1_MODE,                     /* LED mode for LED1, RW */
    RTK_PHY_CTRL_LED_2_MODE,                     /* LED mode for LED2, RW */
    RTK_PHY_CTRL_LED_3_MODE,                     /* LED mode for LED3, RW */
    RTK_PHY_CTRL_LED_4_MODE,                     /* LED mode for LED4, RW */
    RTK_PHY_CTRL_LED_5_MODE,                     /* LED mode for LED5, RW */
    RTK_PHY_CTRL_LED_6_MODE,                     /* LED mode for LED6, RW */
    RTK_PHY_CTRL_LED_CFG_FLASH_RATE,             /* LED flash rate, RW */
    RTK_PHY_CTRL_LED_CFG_BLINK_RATE,             /* LED blink rate, RW */
    RTK_PHY_CTRL_LED_1_CFG_ACTIVE_LOW,           /* active low/high for LED1, RW */
    RTK_PHY_CTRL_LED_2_CFG_ACTIVE_LOW,           /* active low/high for LED2, RW */
    RTK_PHY_CTRL_LED_3_CFG_ACTIVE_LOW,           /* active low/high for LED3, RW */
    RTK_PHY_CTRL_LED_4_CFG_ACTIVE_LOW,           /* active low/high for LED4, RW */
    RTK_PHY_CTRL_LED_5_CFG_ACTIVE_LOW,           /* active low/high for LED5, RW */
    RTK_PHY_CTRL_LED_6_CFG_ACTIVE_LOW,           /* active low/high for LED6, RW */
    RTK_PHY_CTRL_LED_1_CFG_FORCE,                /* LED force mode for LED1, RW */
    RTK_PHY_CTRL_LED_2_CFG_FORCE,                /* LED force mode for LED2, RW */
    RTK_PHY_CTRL_LED_3_CFG_FORCE,                /* LED force mode for LED3, RW */
    RTK_PHY_CTRL_LED_4_CFG_FORCE,                /* LED force mode for LED4, RW */
    RTK_PHY_CTRL_LED_5_CFG_FORCE,                /* LED force mode for LED5, RW */
    RTK_PHY_CTRL_LED_6_CFG_FORCE,                /* LED force mode for LED6, RW */
    RTK_PHY_CTRL_NBASET_802P3BZ_MASK,            /* mask 802.3bz and advert NBASE-T only, RW*/
    RTK_PHY_CTRL_NBASET,                         /* NBASET-T enable/disable, RW */
    RTK_PHY_CTRL_NBASET_STATUS,                  /* link is operated at NBASET-T, R */
    RTK_PHY_CTRL_LOOPBACK_INTERNAL_PMA,          /* internal loopback at PMA, RW */
    RTK_PHY_CTRL_LOOPBACK_REMOTE,                /* remote loopback, RW */
    RTK_PHY_CTRL_SERDES_RX_POLARITY,             /* SerDes RX polarity, RW */
    RTK_PHY_CTRL_SERDES_TX_POLARITY,             /* SerDes TX polarity, RW */
    RTK_PHY_CTRL_MDI_POLARITY_SWAP,              /* MDI polarity swap, RW  */
    RTK_PHY_CTRL_MDI_INVERSE,                    /* MDI pin sequence inverse, RW */
    RTK_PHY_CTRL_RAPID_LINK_FAULT_DETECT,        /* rapid link fault detection enable/disable, RW */
    RTK_PHY_CTRL_PREAMBLE_RECOVERY,              /* preamble-recovery enable, RW */
    RTK_PHY_CTRL_SYNCE,                          /* SyncE enable, RW */
    RTK_PHY_CTRL_SYNCE_0_CLOCK_FREQ,             /* clock frequency for SyncE0, RW */
    RTK_PHY_CTRL_SYNCE_1_CLOCK_FREQ,             /* clock frequency for SyncE1, RW */
    RTK_PHY_CTRL_SYNCE_0_RECOVERY_PHY,           /* recovery PHY port for SyncE0, RW */
    RTK_PHY_CTRL_SYNCE_1_RECOVERY_PHY,           /* recovery PHY port for SyncE1, RW */
    RTK_PHY_CTRL_SYNCE_0_IDLE_MODE,              /* idle mode for SyncE0, RW */
    RTK_PHY_CTRL_SYNCE_1_IDLE_MODE,              /* idle mode for SyncE1, RW */
    RTK_PHY_CTRL_TEMP,                           /* read temperature from thermal meter(bit[18]:sign; [17:10]:integer; [9:0]:decimal), R*/
    RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_LOWER,      /* thermal high threshold, RW([18]:sign; [17:10]:integer(degrees Celsius); [9:0]:decimal), for trigger interrupt when temperature lower then this threshold */
    RTK_PHY_CTRL_TEMP_THRESHOLD_HIGH_HIGHER,     /* thermal high threshold, RW([18]:sign; [17:10]:integer(degrees Celsius); [9:0]:decimal), for trigger interrupt when temperature higher then this threshold */
    RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_LOWER,       /* thermal  low threshold, RW([18]:sign; [17:10]:integer(degrees Celsius); [9:0]:decimal), for trigger interrupt when temperature lower then this threshold */
    RTK_PHY_CTRL_TEMP_THRESHOLD_LOW_HIGHER,      /* thermal  low threshold, RW([18]:sign; [17:10]:integer(degrees Celsius); [9:0]:decimal), for trigger interrupt when temperature higher then this threshold */
    RTK_PHY_CTRL_FAST_RETRAIN,                   /* enable fast retrain ability, RW */
    RTK_PHY_CTRL_FAST_RETRAIN_LPCOUNT,           /* Counts the number of fast retrains requested by the link partner, R */
    RTK_PHY_CTRL_FAST_RETRAIN_LDCOUNT,           /* Counts the number of fast retrains requested by the local device, R */
    RTK_PHY_CTRL_FAST_RETRAIN_STATUS,            /* fast retrain capable at current link(1 for Both local and link-partner have fast retrain capability of current speed), R */
    RTK_PHY_CTRL_FAST_RETRAIN_NFR,               /* enable NFR ability, RW */
    RTK_PHY_CTRL_FAST_RETRAIN_NFR_STATUS,        /* NFR capable at current link, R */
    RTK_PHY_CTRL_COUNTER_CLEAR,                  /* clear counter, W */
    RTK_PHY_CTRL_SERDES_MODE,                    /* config SerDes mode, RW */
    RTK_PHY_CTRL_SERDES_UPDTAE,                  /* update SerDes setting for those modes can't auto change speed when link up(do this after MAC side serdes mode change), W */
    RTK_PHY_CTRL_SERDES_USXGMII_AN,              /* SerDes auto-negotiation enable(1)/disable(0) for USXGMII mode, RW */
    RTK_PHY_CTRL_SERDES_SGMII_AN,                /* SerDes auto-negotiation enable(1)/disable(0) for SGMII mode, RW */
    RTK_PHY_CTRL_SERDES_BASEX_AN,                /* SerDes auto-negotiation enable(1)/disable(0) for 1000BASE-X/2.5GBASE-X/5GBASE-X mode, RW */
    RTK_PHY_CTRL_SERDES_XSGMII_AN,               /* SerDes auto-negotiation enable(1)/disable(0) for XSGMII mode, RW */
    RTK_PHY_CTRL_SNR_CH0,                        /* Get current SNR value(MSE) of Channel 0, R */
    RTK_PHY_CTRL_SNR_CH1,                        /* Get current SNR value(MSE) of Channel 1, R */
    RTK_PHY_CTRL_SNR_CH2,                        /* Get current SNR value(MSE) of Channel 2, R */
    RTK_PHY_CTRL_SNR_CH3,                        /* Get current SNR value(MSE) of Channel 3, R */
    RTK_PHY_CTRL_SNR_THRESHOLD_10G_MASTER,       /* SNR threshold(MSE) for 10G Master, RW */
    RTK_PHY_CTRL_SNR_THRESHOLD_10G_SLAVE,        /* SNR threshold(MSE) for 10G Slave, RW */
    RTK_PHY_CTRL_SNR_THRESHOLD_5G_MASTER,        /* SNR threshold(MSE) for 5G Master, RW */
    RTK_PHY_CTRL_SNR_THRESHOLD_5G_SLAVE,         /* SNR threshold(MSE) for 5G Slave, RW */
    RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_MASTER,      /* SNR threshold(MSE) for 2.5G Master, RW */
    RTK_PHY_CTRL_SNR_THRESHOLD_2P5G_SLAVE,       /* SNR threshold(MSE) for 2.5G Slave, RW */
    RTK_PHY_CTRL_SNR_THRESHOLD_1G,               /* SNR threshold(MSE) for 1G/100M, RW */
    RTK_PHY_CTRL_REINIT,                         /* Set 1 to reset and reinit port, W */
    RTK_PHY_CTRL_SKEW_PAIR_B,                    /* Get current skew delay on pair B with respect to physical pair A*/
    RTK_PHY_CTRL_SKEW_PAIR_C,                    /* Get current skew delay on pair B with respect to physical pair A*/
    RTK_PHY_CTRL_SKEW_PAIR_D,                    /* Get current skew delay on pair B with respect to physical pair A*/
    RTK_PHY_CTRL_SERDES_LOOPBACK_REMOTE,         /* remote loopback on SerDes, RW */
    RTK_PHY_CTRL_RTCT_CABLE_TYPE,                /* RTCT cable type, W */
    RTK_PHY_CTRL_RTCT_CABLE_FACTOR,              /* RTCT cable factor value to caculate cable length, set 0 to use driver default value, RW */
    RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE,          /* serdes mode target for rtk_port_phySdsEyeParam_set/get API, RW */
    RTK_PHY_CTRL_PTP,                            /* PTP enable, RW */
    RTK_PHY_CTRL_PTP_DURATION_THRESHOLD,         /* PTP duration threahold(ns) for trigger time operation (falling edge and rising edge), RW */
    RTK_PHY_CTRL_PTP_REFTIME_TOD_DELAY,          /* PTP ToD delay(ns) for reference time in 16-bit 2's complement format, RW */
    RTK_PHY_CTRL_PTP_PORT_ROLE,                  /* PTP port role, RW */
    RTK_PHY_CTRL_PTP_TX_IMBAL,                   /* PTP TX IMBAL(ns), egressTimestamp = egressMeasuredTimestamp + TX_IMBAL, RW */
    RTK_PHY_CTRL_PTP_RX_IMBAL,                   /* PTP RX IMBAL(ns), ingressTimestamp = ingressMeasuredTimestamp - RX_IMBAL, RW */
    RTK_PHY_CTRL_PTP_CLOCK_SRC,                  /* PTP clock source select external/internal clock, RW */
    RTK_PHY_CTRL_LINKDOWN_CNT,                   /* get link down counter (read clear), R */
    RTK_PHY_CTRL_FATAL_STATUS,                   /* get fatal error status bitmap, R */
    RTK_PHY_CTRL_FATAL_STATUS_READ_CLEAR,        /* get fatal error status bitmap(read clear), RC */
    RTK_PHY_CTRL_PTP_PLL_POW_SRC,                /* PTP PLL power source select, RW*/
    RTK_PHY_CTRL_PTP_PLL_CLK,                    /* PTP PLL clock frequence select, RW*/
    RTK_PHY_CTRL_PTP_PLL,                        /* PTP PLL enable, RW*/
    RTK_PHY_CTRL_EEE_PHYMODE,                    /* EEE PHY-mode enable, RW */
    RTK_PHY_CTRL_MACSEC_BYPASS,                  /* Set 1 to bypass MACsec, RW */
    /* Private/Debug options */
    RTK_PHY_CTRL_DEBUG_DUMP_DSP,                 /* used by rtk_phy_debug_get */
    RTK_PHY_CTRL_DEBUG_DUMP_DSP_INIT,            /* used by rtk_phy_debug_get */
    RTK_PHY_CTRL_DEBUG_DUMP_COUPLING,            /* used by rtk_phy_debug_get */
    RTK_PHY_CTRL_DEBUG_MDI_PLUG,                 /* get MDI link status, R */
    RTK_PHY_CTRL_DEBUG_MDIO_PARITY_CHK,          /* get debug conuter for MDIO parity check, R */
    RTK_PHY_CTRL_DEBUG_CABLE_LENGTH,             /* get linking cable length estimation value(m), R */
    RTK_PHY_CTRL_MIIM_BCAST,                     /* enable/disable miim broadcast, W*/
    RTK_PHY_CTRL_MIIM_BCAST_PHYAD,               /* set miim broadcast PHYAD, W*/
    RTK_PHY_CTRL_SYNCE_PLL,                      /* SyncE output clock source from PLL, W */
    RTK_PHY_CTRL_IPG_SHRINK,                     /* Enable PHY's IPG shrink when switch enables stacking on the port */
    RTK_PHY_CTRL_SERDES_USXGMII_AM_PERIOD,       /* get/set SerDes AM period RW */
    RTK_PHY_CTRL_END
} rtk_phy_ctrl_t;

#define RTK_PHY_CTRL_SYNCE_RECOVERY_PHY  RTK_PHY_CTRL_SYNCE_0_RECOVERY_PHY
#define RTK_PHY_CTRL_SYNCE_CLOCK_FREQ    RTK_PHY_CTRL_SYNCE_0_CLOCK_FREQ
#define RTK_PHY_CTRL_SYNCE_IDLE_MODE     RTK_PHY_CTRL_SYNCE_0_IDLE_MODE

/* value for RTK_PHY_CTRL_LED_X_MODE */
#define RTK_PHY_CTRL_LED_MODE_10G              (1 << 0)      /* LED light when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_10G_LITE         (1 << 1)      /* LED light when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_5G               (1 << 2)      /* LED light when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_5G_LITE          (1 << 3)      /* LED light when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_2P5G             (1 << 4)      /* LED light when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_2P5G_LITE        (1 << 5)      /* LED light when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_1G               (1 << 6)      /* LED light when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_500M             (1 << 7)      /* LED light when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_100M             (1 << 8)      /* LED light when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_10G_FLASH        (1 << 9)      /* LED flash when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_10G_LITE_FLASH   (1 << 10)     /* LED flash when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_5G_FLASH         (1 << 11)     /* LED flash when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_5G_LITE_FLASH    (1 << 12)     /* LED flash when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_2P5G_FLASH       (1 << 13)     /* LED flash when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_2P5G_LITE_FLASH  (1 << 14)     /* LED flash when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_1G_FLASH         (1 << 15)     /* LED flash when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_500M_FLASH       (1 << 16)     /* LED flash when link up at specific speed at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_100M_FLASH       (1 << 17)     /* LED flash when link up at specific speed */
#define RTK_PHY_CTRL_LED_MODE_RX_ACT           (1 << 18)     /* LED blink when packet receiving */
#define RTK_PHY_CTRL_LED_MODE_TX_ACT           (1 << 19)     /* LED blink when packet transmitting */
#define RTK_PHY_CTRL_LED_MODE_LITE_FLASH       (1 << 20)     /* LED flash when link at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_LITE             (1 << 21)     /* LED light when link at two-pair mode */
#define RTK_PHY_CTRL_LED_MODE_DUPLEX           (1 << 22)     /* LED light when link at full duplex */
#define RTK_PHY_CTRL_LED_MODE_MASTER           (1 << 23)     /* LED light when link at master mode*/
#define RTK_PHY_CTRL_LED_MODE_TRAINING         (1 << 24)     /* LED blink when PHY at training state*/
#define RTK_PHY_CTRL_LED_MODE_LINK_EN          (1 << 25)     /* LED light when link up, unset this bit for power-saving */
#define RTK_PHY_CTRL_LED_MODE_LINK_FLASH       (1 << 26)     /* LED flash when link up*/


/* value for RTK_PHY_CTRL_LED_CFG_FLASH_RATE */
#define RTK_PHY_CTRL_LED_CFG_FLASH_RATE_128MS  0
#define RTK_PHY_CTRL_LED_CFG_FLASH_RATE_256MS  1
#define RTK_PHY_CTRL_LED_CFG_FLASH_RATE_512MS  2
#define RTK_PHY_CTRL_LED_CFG_FLASH_RATE_1024MS 3

/* value for RTK_PHY_CTRL_LED_CFG_BLINK_RATE */
#define RTK_PHY_CTRL_LED_CFG_BLINK_RATE_32MS   0
#define RTK_PHY_CTRL_LED_CFG_BLINK_RATE_64MS   1
#define RTK_PHY_CTRL_LED_CFG_BLINK_RATE_128MS  2
#define RTK_PHY_CTRL_LED_CFG_BLINK_RATE_256MS  3
#define RTK_PHY_CTRL_LED_CFG_BLINK_RATE_512MS  4
#define RTK_PHY_CTRL_LED_CFG_BLINK_RATE_1024MS 5

/* value for RTK_PHY_CTRL_LED_X_CFG_FORCE*/
#define RTK_PHY_CTRL_LED_CFG_FORCE_DISABLE     0 /* trun off cpu force mode */
#define RTK_PHY_CTRL_LED_CFG_FORCE_ON          1
#define RTK_PHY_CTRL_LED_CFG_FORCE_BLINK       2
#define RTK_PHY_CTRL_LED_CFG_FORCE_FLASH       3
#define RTK_PHY_CTRL_LED_CFG_FORCE_OFF         4

/* value for RTK_PHY_CTRL_INTR_MASK*/
#define RTK_PHY_CTRL_INTR_MASK_COMMON          (1 << RTK_PHY_INTR_COMMON)
#define RTK_PHY_CTRL_INTR_MASK_RLFD            (1 << RTK_PHY_INTR_RLFD)
#define RTK_PHY_CTRL_INTR_MASK_TM_LOW          (1 << RTK_PHY_INTR_TM_LOW)
#define RTK_PHY_CTRL_INTR_MASK_TM_HIGH         (1 << RTK_PHY_INTR_TM_HIGH)
#define RTK_PHY_CTRL_INTR_MASK_MACSEC          (1 << RTK_PHY_INTR_MACSEC)


/* value for RTK_PHY_CTRL_INTR_ENABLE & RTK_PHY_CTRL_INTR_STATUS*/
#define RTK_PHY_CTRL_INTR_NEXT_PAGE_RECV       (1 << RTK_PHY_INTR_STATUS_COMMON_NWAY_NEXT_PAGE_RECV)
#define RTK_PHY_CTRL_INTR_AN_COMPLETE          (1 << RTK_PHY_INTR_STATUS_COMMON_AN_COMPLETE)
#define RTK_PHY_CTRL_INTR_LINK_CHANGE          (1 << RTK_PHY_INTR_STATUS_COMMON_LINK_CHANGE)
#define RTK_PHY_CTRL_INTR_ALDPS_STATE_CHANGE   (1 << RTK_PHY_INTR_STATUS_COMMON_ALDPS_STATE_CHANGE)
#define RTK_PHY_CTRL_INTR_RLFD                 (1 << RTK_PHY_INTR_STATUS_RLFD)
#define RTK_PHY_CTRL_INTR_TM_LOW               (1 << RTK_PHY_INTR_STATUS_TM_LOW)
#define RTK_PHY_CTRL_INTR_TM_HIGH              (1 << RTK_PHY_INTR_STATUS_TM_HIGH)
#define RTK_PHY_CTRL_INTR_FATAL_ERROR          (1 << RTK_PHY_INTR_STATUS_FATAL_ERROR)
#define RTK_PHY_CTRL_INTR_MACSEC               (1 << RTK_PHY_INTR_STATUS_MACSEC)
#define RTK_PHY_CTRL_INTR_PTP1588              (1 << RTK_PHY_INTR_STATUS_PTP1588)

/* value for RTK_PHY_CTRL_MDI_POLARITY_SWAP */
#define RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_A    0b0001
#define RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_B    0b0010
#define RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_C    0b0100
#define RTK_PHY_CTRL_MDI_POLARITY_SWAP_CH_D    0b1000

/* value for RTK_PHY_CTRL_SYNCE_X_CLOCK_FREQ */
#define RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_50MHZ    0
#define RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_25MHZ    1
#define RTK_PHY_CTRL_SYNCE_CLOCK_FREQ_8KHZ     2

/* value for RTK_PHY_CTRL_SYNCE_X_IDLE_MODE */
#define RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOCAL_FREERUN 0
#define RTK_PHY_CTRL_SYNCE_IDLE_MODE_LOW           1
#define RTK_PHY_CTRL_SYNCE_IDLE_MODE_HIGH          2

/* value for RTK_PHY_CTRL_SERDES_MODE */
#define RTK_PHY_CTRL_SERDES_MODE_OFF                                  0x00FF  /* POWER OFF */
#define RTK_PHY_CTRL_SERDES_MODE_USXGMII                              0x0000  /* USXGMII */
#define RTK_PHY_CTRL_SERDES_MODE_10GR_5GX_2P5GX_SGMII                 0x1001  /* 10G-R/5G-X/2.5G-X/SGMII */
#define RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GADAPT_XFI2P5GADAPT_SGMII   0x1002  /* 10G-R/XFI-5G-ADAPT/XFI-2.5G-ADAPT/SGMII */
#define RTK_PHY_CTRL_SERDES_MODE_10GR_5GR_2P5GX_SGMII                 0x1101  /* 10G-R/5G-R/2.5G-X/SGMII */
#define RTK_PHY_CTRL_SERDES_MODE_10GR_XFI5GCPRI_2P5GX_SGMII           0x1102  /* 10G-R/XFI-5G-CPRI/2.5G-X/SGMII */
#define RTK_PHY_CTRL_SERDES_MODE_UNKNOWN                              0xFFFF
#define RTK_PHY_CTRL_SERDES_MODE_IS_AUTO(_mode)             ((_mode & 0x0100) == 0x0)

/* value for RTK_PHY_CTRL_FAST_RETRAIN */
#define RTK_PHY_CTRL_FAST_RETRAIN_DISABLE  0
#define RTK_PHY_CTRL_FAST_RETRAIN_ENABLE   1

/* value for RTK_PHY_CTRL_FAST_RETRAIN_NFR */
#define RTK_PHY_CTRL_FAST_RETRAIN_NFR_DISABLE  0
#define RTK_PHY_CTRL_FAST_RETRAIN_NFR_ENABLE   1

#define RTK_PHY_CTRL_DEBUG_DUMP_ITEM(_cnt, _idx) (((_cnt & 0xFFFF) <<16) | (_idx & 0xFFFF))
#define RTK_PHY_CTRL_DEBUG_DUMP_CNT(_val)         ((_val & (0xFFFF0000)) >> 16)
#define RTK_PHY_CTRL_DEBUG_DUMP_IDX(_val)         (_val & 0xFFFF)

/* value for RTK_PHY_CTRL_RTCT_CABLE_TYPE */
#define RTK_PHY_CTRL_RTCT_CABLE_TYPE_NONE       0
#define RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT5E      1
#define RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A      2
#define RTK_PHY_CTRL_RTCT_CABLE_TYPE_CAT6A_UU   3

/* RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE */
#define RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GR     RTK_MII_5GR
#define RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_5GX     RTK_MII_5GBASEX
#define RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_2P5GX   RTK_MII_2500Base_X
#define RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_1GX     RTK_MII_SGMII
#define RTK_PHY_CTRL_SERDES_EYE_PARAM_TYPE_DEFAULT 0

/* value for RTK_PHY_CTRL_PTP_PORT_ROLE */
#define RTK_PHY_CTRL_PTP_PORT_ROLE_NONE      0x0 /* Non PTP port */
#define RTK_PHY_CTRL_PTP_PORT_ROLE_BC_OC     0x1 /* Boundary Clock or Ordinary Clock*/
#define RTK_PHY_CTRL_PTP_PORT_ROLE_E2E_TC    0x2 /* E2E Transparent Clock */
#define RTK_PHY_CTRL_PTP_PORT_ROLE_P2P_TC    0x3 /* P2P Transparent Clock */

/* value for RTK_PHY_CTRL_PTP_CLOCK_SRC */
#define RTK_PHY_CTRL_PTP_CLOCK_SRC_INT       0x0 /* Internal Clock */
#define RTK_PHY_CTRL_PTP_CLOCK_SRC_EXT       0x1 /* External Clock*/

/* value for RTK_PHY_CTRL_FATAL_STATUS */
#define RTK_PHY_CTRL_FATAL_STATUS_UC_PC_OUT_OF_RANGE    ( 0x1 << 0 )
#define RTK_PHY_CTRL_FATAL_STATUS_UC_RESPONSE_FAILED    ( 0x1 << 1 )
#define RTK_PHY_CTRL_FATAL_STATUS_NCTL_PC_OUT_OF_RANGE  ( 0x1 << 2 )
#define RTK_PHY_CTRL_FATAL_STATUS_NCTL_PC_STUCK         ( 0x1 << 3 )
#define RTK_PHY_CTRL_FATAL_STATUS_SYSCLK_DETECT_FAILED  ( 0x1 << 4 )
#define RTK_PHY_CTRL_FATAL_STATUS_MAC_CK250M_FAILED     ( 0x1 << 5 )
#define RTK_PHY_CTRL_FATAL_STATUS_SRAM_ECC_1BITS_ERROR  ( 0x1 << 6 )
#define RTK_PHY_CTRL_FATAL_STATUS_SRAM_ECC_2BITS_ERROR  ( 0x1 << 7 )
#define RTK_PHY_CTRL_FATAL_STATUS_UC_STACK_FAILED       ( 0x1 << 8 )

/* value for RTK_PHY_CTRL_PTP_PLL_POW_SRC */
#define RTK_PHY_CTRL_PTP_PLL_POW_SRC_EXT      0x0 /* PLL use external power */
#define RTK_PHY_CTRL_PTP_PLL_POW_SRC_LDO      0x1 /* PLL use LDO power */

/* value for RTK_PHY_CTRL_PTP_PLL_CLK */
#define RTK_PHY_CTRL_PTP_PLL_CLK_UNKNOWN      0xFFFF
#define RTK_PHY_CTRL_PTP_PLL_CLK_NONE         0x0
#define RTK_PHY_CTRL_PTP_PLL_CLK_25MHZ        0x1
#define RTK_PHY_CTRL_PTP_PLL_CLK_50MHZ        0x2
#define RTK_PHY_CTRL_PTP_PLL_CLK_125MHZ       0x3



/* RTCT result Status of one channel */
typedef struct rtk_rtctChannelStatus_s
{
    uint32      channelShort;
    uint32      channelOpen;
    uint32      channelLowMismatch;
    uint32      channelHighMismatch;
    uint32      channelCrossoverA;
    uint32      channelCrossoverB;
    uint32      channelCrossoverC;
    uint32      channelCrossoverD;
    uint32      channelLen;
} rtk_rtctChannelStatus_t;

/* result of RTCT test */
typedef struct rtk_rtctResult_s
{
    rtk_port_speed_t    linkType;
    union
    {
        struct fe_result_s
        {
            uint32      isRxShort;
            uint32      isTxShort;
            uint32      isRxOpen;
            uint32      isTxOpen;
            uint32      isRxMismatch;
            uint32      isTxMismatch;
            uint32      isRxLinedriver;
            uint32      isTxLinedriver;
            uint32      rxLen;
            uint32      txLen;
        } fe_result;

        struct ge_result_s
        {
            uint32      channelAShort;
            uint32      channelBShort;
            uint32      channelCShort;
            uint32      channelDShort;

            uint32      channelAOpen;
            uint32      channelBOpen;
            uint32      channelCOpen;
            uint32      channelDOpen;

            uint32      channelAMismatch;
            uint32      channelBMismatch;
            uint32      channelCMismatch;
            uint32      channelDMismatch;

            uint32      channelALinedriver;
            uint32      channelBLinedriver;
            uint32      channelCLinedriver;
            uint32      channelDLinedriver;

            uint32      channelAHiImpedance;
            uint32      channelBHiImpedance;
            uint32      channelCHiImpedance;
            uint32      channelDHiImpedance;

            uint32      channelACross;
            uint32      channelBCross;
            uint32      channelCCross;
            uint32      channelDCross;

            uint32      channelAPartialCross;
            uint32      channelBPartialCross;
            uint32      channelCPartialCross;
            uint32      channelDPartialCross;

            /* when paire is busy, RTCT shall re-do by calling start API again. */
            uint32      channelAPairBusy;
            uint32      channelBPairBusy;
            uint32      channelCPairBusy;
            uint32      channelDPairBusy;

            uint32      channelALen;
            uint32      channelBLen;
            uint32      channelCLen;
            uint32      channelDLen;
        } ge_result;

        struct channels_result_s
        {
            rtk_rtctChannelStatus_t     a;
            rtk_rtctChannelStatus_t     b;
            rtk_rtctChannelStatus_t     c;
            rtk_rtctChannelStatus_t     d;
        } channels_result;
    } un;
} rtk_rtctResult_t;


typedef enum rtk_phy_debug_type_e
{
    RTK_PHY_DEBUG_DSP,  //you should reboot after execute this debug function
    RTK_PHY_DEBUG_DSP_MULTIPORT,  //you should reboot after execute this debug function
    RTK_PHY_DEBUG_COUPLING, //you should reboot after execute this debug function
} rtk_phy_debug_type_t;

typedef struct rtk_phy_debug_s
{
    rtk_phy_debug_type_t  phyDbgType;
    rtk_port_t            port;
    union
    {
        struct phy_debug_dsp_s
        {
            uint32 cnt;  //dump counts
            uint64 itemBitmap;  //a bitmap to select items to dump
        } dsp;
        struct phy_debug_dsp_multiport_s
        {
            uint32 cnt;  //dump counts
            uint64 itemBitmap;  //a bitmap to select items to dump
            rtk_portmask_t portmask; //a portmask to asign multi-ports to dump(instead of rtk_phy_debug_t.port)
        } dsp_multiport;
        struct phy_debug_coupling_s
        {
            uint32 channel_bitmap; //a bitmap of target channel, 0 for run on all channels.
        } coupling;
    } parameter;
} rtk_phy_debug_t;

typedef enum rtk_phy_batch_op_e {
    RTK_PHY_BATCH_OP_EMPTY = 0, // empty entry
    RTK_PHY_BATCH_OP_LOOPSTART, // start of loop
    RTK_PHY_BATCH_OP_LOOPEND,   // end of loop
    RTK_PHY_BATCH_OP_W_C45,     // write clause 45 REG
    RTK_PHY_BATCH_OP_R_C45,     // read clause 45 REG
    RTK_PHY_BATCH_OP_C_C45,     // compare clause 45 REG
    RTK_PHY_BATCH_OP_WEQ_C45,   // wait clause 45 REG equal to specific value
    RTK_PHY_BATCH_OP_WNEQ_C45,  // wait clause 45 REG not equal to specific value
    RTK_PHY_BATCH_OP_W_C22,     // write clause 22 REG
    RTK_PHY_BATCH_OP_R_C22,     // read clause 22 REG
    RTK_PHY_BATCH_OP_C_C22,     // compare clause 22 REG
    RTK_PHY_BATCH_OP_INIT,      // init batch profile
    RTK_PHY_BATCH_OP_RUN,       // run batch profile
    RTK_PHY_BATCH_OP_DUMP,      // dump batch profile
} rtk_phy_batch_op_t;

#define RTK_PHY_DEBUG_BATCH_OP_LEN 20

typedef struct rtk_phy_batch_para_s
{
    rtk_phy_batch_op_t batch_op;
    uint8  para_num;
    uint32 para1;
    uint32 para2;
    uint32 para3;
    uint32 para4;
    uint32 para5;
} rtk_phy_batch_para_t;

typedef struct rtk_phy_batch_entry_s
{
    rtk_phy_batch_op_t batch_op;
    uint8  para_num;
    uint32 para[5];
} rtk_phy_batch_entry_t;


/* MACSec */
#define RTK_MACSEC_MAX_KEY_LEN        32

typedef enum rtk_macsec_reg_e
{
    RTK_MACSEC_DIR_EGRESS = 0,
    RTK_MACSEC_DIR_INGRESS,
    RTK_MACSEC_DIR_END,
} rtk_macsec_dir_t;

typedef enum rtk_macsec_an_e
{
    RTK_MACSEC_AN0 = 0,
    RTK_MACSEC_AN1,
    RTK_MACSEC_AN2,
    RTK_MACSEC_AN3,
    RTK_MACSEC_AN_MAX,
} rtk_macsec_an_t ;

typedef enum rtk_macsec_flow_e
{
    RTK_MACSEC_FLOW_BYPASS = 0,
    RTK_MACSEC_FLOW_DROP,
    RTK_MACSEC_FLOW_INGRESS,
    RTK_MACSEC_FLOW_EGRESS,
} rtk_macsec_flow_type_t;

typedef enum rtk_macsec_validate_e
{
    RTK_MACSEC_VALIDATE_STRICT = 0,
    RTK_MACSEC_VALIDATE_CHECK,
    RTK_MACSEC_VALIDATE_DISABLE,
} rtk_macsec_validate_t;

typedef enum rtk_macsec_cipher_e
{
    RTK_MACSEC_CIPHER_GCM_ASE_128 = 0,
    RTK_MACSEC_CIPHER_GCM_ASE_256,
    RTK_MACSEC_CIPHER_GCM_ASE_XPN_128,
    RTK_MACSEC_CIPHER_GCM_ASE_XPN_256,
    RTK_MACSEC_CIPHER_MAX,
} rtk_macsec_cipher_t;

typedef enum rtk_macsec_stat_e
{
    RTK_MACSEC_STAT_InPktsUntagged = 0,
    RTK_MACSEC_STAT_InPktsNoTag,
    RTK_MACSEC_STAT_InPktsBadTag,
    RTK_MACSEC_STAT_InPktsUnknownSCI,
    RTK_MACSEC_STAT_InPktsNoSCI,
    RTK_MACSEC_STAT_OutPktsUntagged,
    RTK_MACSEC_STAT_MAX,
} rtk_macsec_stat_t;

typedef enum rtk_macsec_txsa_stat_e
{
    RTK_MACSEC_TXSA_STAT_OutPktsTooLong = 0,
    RTK_MACSEC_TXSA_STAT_OutOctetsProtectedEncrypted,
    RTK_MACSEC_TXSA_STAT_OutPktsProtectedEncrypted,
    RTK_MACSEC_TXSA_STAT_MAX,
} rtk_macsec_txsa_stat_t;

typedef enum rtk_macsec_rxsa_stat_e
{
    RTK_MACSEC_RXSA_STAT_InPktsUnusedSA = 0,
    RTK_MACSEC_RXSA_STAT_InPktsNotUsingSA,
    RTK_MACSEC_RXSA_STAT_InPktsUnchecked,
    RTK_MACSEC_RXSA_STAT_InPktsDelayed,
    RTK_MACSEC_RXSA_STAT_InPktsLate,
    RTK_MACSEC_RXSA_STAT_InPktsOK,
    RTK_MACSEC_RXSA_STAT_InPktsInvalid,
    RTK_MACSEC_RXSA_STAT_InPktsNotValid,
    RTK_MACSEC_RXSA_STAT_InOctetsDecryptedValidated,
    RTK_MACSEC_RXSA_STAT_MAX,
} rtk_macsec_rxsa_stat_t;

/* enable, the packet flow go through MACsec logic */
#define RTK_MACSEC_PORT_F_ENABLE             0x00000001U
/* drop non-ctrl frame with KaY tag(C=0/E=1), should not enable when KaY is used for key agressment) */
#define RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY   0x00000002U

typedef struct rtk_macsec_port_cfg_s
{
    /* a bitmap of RTK_MACSEC_PORT_F_* */
    uint32 flags;

    /* Egress PX/XPN threshold value to trigger interrupt.
       With this interrupt, the system has time to start a re-key before the rollover error fires,
       default is 0, interrupt trigger when PN/XPN rollover*/
    uint32 pn_intr_threshold; //only for cipher suites with 32-bit packet numbering
    uint64 xpn_intr_threshold; //only for cipher suites with 64-bit packet numbering
    /*
       counter threshold value for trigger interrupt.
    */
    uint64 frame_cnt_intr_threshold;
    uint64 octet_cnt_intr_threshold;

    /*
       This value is used for 802.1AE compliant processing when no MACsec secure channel is found.
       If a MACsec secure channel was found, the validate_frames field from the corresponding RXSC is used.
    */
    rtk_macsec_validate_t nm_validate_frames;
}
rtk_macsec_port_cfg_t;

typedef enum rtk_macsec_match_tx_e
{
    RTK_MACSEC_MATCH_NON_CTRL = 0, /* match all non-control and untagged packets */
    RTK_MACSEC_MATCH_MAC_DA, /* match all non-control and untagged packets with specific MAC DA */
} rtk_macsec_match_tx_t;

typedef struct rtk_macsec_txsc_s
{
    /* 8-byte SCI([0:5] = MAC address, [6:7] = port index) for this secure channel  */
    uint8      sci[8];

    /* cipher suite for this SC */
    rtk_macsec_cipher_t    cipher_suite;

    /* packet flow type to match this SC */
    rtk_macsec_match_tx_t  flow_match;
    rtk_mac_t  mac_da; /* the target address for RTK_MACSEC_MATCH_MAC_DA */

    uint8 protect_frame;  /* 1 = enable frame protection */
    uint8 include_sci;    /* 1 = include explicit SCI in packet */
    uint8 use_es;         /* 1 = set ES (End Station) bit in TCI field */
    uint8 use_scb;        /* 1 = set SCB (Single Copy Broadcast) bit in TCI field */
    uint8 conf_protect;   /* 1 = enable confidentiality protection, */
}rtk_macsec_txsc_t;

typedef enum rtk_macsec_match_rx_e
{
    RTK_MACSEC_MATCH_SCI = 0,
    RTK_MACSEC_MATCH_MAC_SA,  //for pkt without SCI field/TCI.SC=0,
} rtk_macsec_match_rx_t;

typedef struct rtk_macsec_rxsc_s
{
    /* 8-byte SCI([0:5] = MAC address, [6:7] = port index) for this secure channel  */
    uint8      sci[8];

    /* cipher suite for this SC */
    rtk_macsec_cipher_t   cipher_suite;

    /* packet flow type to match this SC */
    rtk_macsec_match_rx_t flow_match;
    rtk_mac_t  mac_sa; /* the target address for RTK_MACSEC_MATCH_MAC_SA */

    /* frame validation level */
    rtk_macsec_validate_t validate_frames;

    /* replay protection */
    uint8  replay_protect;  /* 1 = enable replay protection */
    uint32 replay_window;   /* the window size for replay protection, range for PN: 0 ~ 2^32 - 1, for XPN: 0 ~ 2^30 */

}rtk_macsec_rxsc_t;

typedef union rtk_macsec_sc_u
{
    rtk_macsec_txsc_t tx;
    rtk_macsec_rxsc_t rx;
}
rtk_macsec_sc_t;


typedef struct rtk_macsec_txsc_status_s
{
    uint32 hw_flow_index;
    uint32 hw_sa_index;
    uint8  sa_inUse;
    uint32 hw_flow_data;
    uint8  hw_sc_flow_status;
    rtk_macsec_an_t running_an;
}
rtk_macsec_txsc_status_t;

typedef struct rtk_macsec_rxsc_status_s
{
    uint32 hw_flow_base;
    uint32 hw_sa_index[RTK_MACSEC_AN_MAX];
    uint8  sa_inUse[RTK_MACSEC_AN_MAX];
    uint32 hw_flow_data[RTK_MACSEC_AN_MAX];
    uint8  hw_sc_flow_status[RTK_MACSEC_AN_MAX];
}
rtk_macsec_rxsc_status_t;

typedef union rtk_macsec_sc_status_u
{
    rtk_macsec_txsc_status_t tx;
    rtk_macsec_rxsc_status_t rx;
}
rtk_macsec_sc_status_t;

typedef struct rtk_macsec_sa_s
{
    uint8 key[RTK_MACSEC_MAX_KEY_LEN];  // MACsec Key.
    uint32 key_bytes; // Size of the MACsec key in bytes (16 for AES128, 32 for AES256).

    uint32 pn;      // PN (32-bit) or lower 32-bit of XPN (64-bit)
    uint32 pn_h;    // higher 32-bit of XPN (64-bit)
    uint8 salt[12]; // 12-byte salt (for XPN).
    uint8 ssci[4];  // 4-byte SSCI value (for XPN).
} rtk_macsec_sa_t;

#define RTK_MACSEC_INTR_EGRESS_PN_THRESHOLD          0x00000001
#define RTK_MACSEC_INTR_EGRESS_PN_ROLLOVER           0x00000002

typedef struct rtk_macsec_intr_status_s
{
    /* a bitmap of RTK_MACSEC_INTR_*  to present occured event */
    uint32 status;

    /* When read 1b, the corresponding MACsec egress SA is about to expire due to
       the packet number crossing the rtk_macsec_port_cfg_t.pn_intr_threshold or xpn_intr_threshold*/
    uint8  egress_pn_thr_an_bmap[RTK_MAX_MACSEC_SC_PER_PORT]; //bitmap of AN3~0.

    /* When read 1b, the corresponding MACsec egress SA has expired due to
       the packet number reaching the maximum allowed value. */
    uint8  egress_pn_exp_an_bmap[RTK_MAX_MACSEC_SC_PER_PORT]; //bitmap of AN3~0.
}
rtk_macsec_intr_status_t;

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


/* Function Name:
 *      rtk_phy_init
 * Description:
 *      Initialize phy module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      Must initialize port module before calling any port APIs.
 * Changes:
 *      None
 */
int32
rtk_phy_init(uint32 unit);


/* Module Name    : PHY                                       */
/* Sub-module Name: Parameter settings for the port-based view */


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
 *      8261, 8264, 8380, 8390, 9300, 9310
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
int32
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
 *      8261, 8264, 8380, 8390, 9300, 9310
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
int32
rtk_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex);

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
int32
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
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS, 8226, 8226B
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely switch the mode without calling ability setting API again
 * Changes:
 *      None
 */
int32
rtk_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyAutoNegoAbilityLocal_get
 * Description:
 *      Get complete abilities for auto negotiation of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phyAutoNegoAbilityLocal_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS, 8226, 8226B
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8390SDS, 8380SDS, 9310SDS
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
int32
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
int32
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
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      This function only works on giga/ 10g port to set its master/slave mode configuration.
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 9300SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 9300SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8380, 8390, 9300
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
int32
rtk_port_phymaskMmdReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data);

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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 9300SDS, 8390SDS, 8380SDS
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
int32
rtk_port_phyComboPortMedia_set(uint32 unit, rtk_port_t port, rtk_port_media_t media);

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
 *      8261, 8264, 8208G, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      Needs to restart auto-negotiation for the configuration to take effect.
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      The valid value of mode is as following:
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      The valid value of mode is as following:
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8214C, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      The valid value of mode is as following:
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 * Changes:
 *      None
 */
int32
rtk_port_phyCrossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);

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
int32
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
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_port_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

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
 *      8214C, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8214C, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8226, 8226B, 8284
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
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
 *      8226, 8226B, 8284
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Applicable:
 *      8261, 8264, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Applicable:
 *      8261, 8264, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_port_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

/* Function Name:
 *      rtk_port_downSpeedStatus_get
 * Description:
 *      Get the statue of down speed status of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pDownSpeedStatus - pointer to status of down speed.
 *                         TRUE: link is up due to down speed; FALSE: down speed is not performed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Applicable:
 *      8261, 8264, 8218B, 8218FB, 8214FC, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_downSpeedStatus_get(uint32 unit, rtk_port_t port, uint32 *pDownSpeedStatus);

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
int32
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
int32
rtk_port_fiberNwayForceLinkEnable_set(uint32 unit, rtk_port_t port,
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
 *      8261, 8264, 8214C, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
 *      8261, 8264, 8214C, 8218B, 8218FB, 8214FC, 8295R, 8214QF, 8218D, 9300SDS, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
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
int32
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
int32
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
int32
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
int32
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
int32
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
int32
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
 *      8261, 8264, 8218B, 8214FC, 8218FB, 8214FB, 8214B, 8212B, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
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
int32
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
int32
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
int32
rtk_port_phyEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum);

/* Function Name:
 *      rtk_port_phyEyeMonitorInfo_get
 * Description:
 *      Get the statue of eye monitor height and width of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - base port id of the PHY
 *      sds     - SerDes id of the PHY
 *      frameNum- frame number
 * Output:
 *      pInfo - pointer to the information of eye monitor height and width
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_BUSYWAIT_TIMEOUT - Read information timeout
 * Applicable:
 *      8218D, 8295R, 8214QF
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyEyeMonitorInfo_get(uint32 unit, rtk_port_t port, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo);

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
 *      [SDK_3.2.0]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D
 * Note:
 *      The Link Status bit (Status Register 1.2) has LL (Latching Low) attribute
 *      for link failure. Please refer IEEE 802.3 for detailed.
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.1]
 *          New added function.
 */
int32
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
int32
rtk_port_phyLedMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode);

/* Function Name:
 *      rtk_port_phyLedCtrl_set
 * Description:
 *      Ger Configuration of LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pLedCtrl - PHY LED control
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
int32
rtk_port_phyLedCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

/* Function Name:
 *      rtk_port_phyLedCtrl_set
 * Description:
 *      Configure LED for PHY control LED
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 *      pLedCtrl - PHY LED control
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
int32
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
 *      8261, 8264, 8218D, 8226, 8226B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.4.2]
 *         New added function.
 */
int32
rtk_port_phyMacIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port,
    rtk_phy_macIntfSdsLinkStatus_t *pStatus);

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
 *      8261, 8264, 8218D, 8214QF, 8295R
 * Note:
 *      None
 * Changes:
 *      [SDK_3.5.0]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D, 8214QF, 8295R
 * Note:
 *      None
 * Changes:
 *      [SDK_3.5.0]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
int32
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
 *      8261, 8264, 8218D
 * Note:
 *      Require setting cross-over to MDI or MDIX.
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
int32
rtk_port_phyMdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyIntr_init
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
 * Applicable:
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyIntr_init(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr);

/* Function Name:
 *      rtk_port_phyIntrEnable_get
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
 * Applicable:
 *      8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyIntrEnable_get(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_phyIntrEnable_set
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
 * Applicable:
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyIntrEnable_set(uint32 unit, rtk_port_t port, rtk_phy_intr_status_t phyIntr, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyIntrStatus_get
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
 * Applicable:
 *      8261, 8264, 8218D
 * Note:
 *      The PHY interrupt status register is read-clear.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyIntrStatus_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, rtk_phy_intrStatusVal_t *pStatus);

/* Function Name:
 *      rtk_port_phyIntrMask_get
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
 * Applicable:
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyIntrMask_get(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 *pMask);

/* Function Name:
 *      rtk_port_phyIntrMask_set
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
 * Applicable:
 *      8261, 8264, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phyIntrMask_set(uint32 unit, rtk_port_t port, rtk_phy_intr_t phyIntr, uint32 mask);

/* Function Name:
 *      rtk_port_phySdsTestMode_set
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
 * Applicable:
 *      8261, 8264, 8218D, 8214QF
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phySdsTestMode_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_testMode_t testMode);

/* Function Name:
 *      rtk_port_phySdsTestModeCnt_get
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
 * Applicable:
 *      8261, 8264, 8218D, 8214QF
 * Note:
 *      The test pattern error counter register is read-clear.
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phySdsTestModeCnt_get(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 *pCnt);

/* Function Name:
 *      rtk_port_phySdsLeq_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8214QF, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phySdsLeq_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t *pManual_en, uint32 *pLeq_val);

/* Function Name:
 *      rtk_port_phySdsLeq_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - input parameter out of range
 * Applicable:
 *      8214QF, 8218D
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_port_phySdsLeq_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_enable_t manual_en, uint32 leq_val);

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
int32
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
int32
rtk_port_phySds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

/* Function Name:
 *      rtk_port_phyCtrl_get
 * Description:
 *      Get Port/PHY specific settings
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      ctrl_type - setting type
 * Output:
 *      pValue    - pointer to setting value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phyCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 *pValue);

/* Function Name:
 *      rtk_port_phyCtrl_set
 * Description:
 *      Set Port/PHY specific settings
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
 * Applicable:
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phyCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ctrl_t ctrl_type, uint32 value);

/* Function Name:
 *      rtk_port_phyLiteEnable_get
 * Description:
 *      Get the status of Lite setting
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mode    - Lite speed mode
 * Output:
 *      pEnable - pointer to status of Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phyLiteEnable_get(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_port_phyLiteEnable_set
 * Description:
 *      Set the status of Lite setting
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      mode    - Lite speed mode
 *      enable  - status of Lite
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
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phyLiteEnable_set(uint32 unit, rtk_port_t port, rtk_port_lite_mode_t mode, rtk_enable_t enable);

/* Function Name:
 *      rtk_port_phyDbgCounter_get
 * Description:
 *      Get the status of debug counter
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - counter type
 * Output:
 *      pCnt - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      [SDK_3.8.0]
 *          New added function.
 */
int32
rtk_port_phyDbgCounter_get(uint32 unit, rtk_port_t port, rtk_port_phy_dbg_cnt_t type, uint64 *pCnt);

/* Function Name:
 *      rtk_phy_debug_get
 * Description:
 *      Dump RealTek PHY debug information
 * Input:
 *      unit  - unit id
 *      pDbg  - pointer to the parameter structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      8261, 8264
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_phy_debug_get(uint32 unit, rtk_phy_debug_t *pDbg);

/* Function Name:
 *      rtk_eee_portEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_eee_portEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_eee_portEnable_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8261, 8264, 8380, 8390, 9300, 9310
 * Note:
 *      Needs to restart auto-negotiation for the configuration to take effect.
 * Changes:
 *      None
 */
int32
rtk_eee_portEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_diag_portRtctResult_get
 * Description:
 *      Get test result of Realtek Cable Tester.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT             - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Applicable:
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D
 * Note:
 *      (1) If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *          If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 *      (2) The unit of cable lenght is centimeter.
 * Changes:
 *      [3.2.0]
 *          rtk_rtctResult_t adds HiImpedance, Cross, PartialCross, and PairBusy.
 */
extern int32
rtk_diag_portRtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);

/* Function Name:
 *      rtk_diag_rtctEnable_set
 * Description:
 *      Start Realtek Cable Tester for ports.
 * Input:
 *      unit      - unit id
 *      pPortmask - the ports for RTCT test
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8261, 8264, 8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D
 * Note:
 *      When RTCT starts, the port won't transmit and receive normal traffic.
 * Changes:
 *      None
 */
extern int32
rtk_diag_rtctEnable_set(uint32 unit, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_phy_debug_batch_port_set
 * Description:
 *      Set target port list for debug batch tool
 * Input:
 *      unit      - unit id
 *      pPortmask - the ports for debug batch
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *
 * Note:
 *
 * Changes:
 *      [SDK_3.6.5]
 *          New added function.
 */
extern int32
rtk_phy_debug_batch_port_set(uint32 unit, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_phy_debug_batch_op_set
 * Description:
 *      debug batch command
 * Input:
 *      pPara     - input parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *
 * Note:
 *
 * Changes:
 *      [SDK_3.6.5]
 *          New added function.
 */
extern int32
rtk_phy_debug_batch_op_set(uint32 unit, rtk_phy_batch_para_t *pPara);


/* Function Name:
 *      rtk_time_portPtpEnable_get
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
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_portPtpEnable_set
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
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_portPtpRxTimestamp_get
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
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_time_portPtpRxTimestamp_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        *pTimeStamp);

/* Function Name:
 *      rtk_time_portPtpTxTimestamp_get
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
 * Applicable:
 *      8390, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_time_portPtpTxTimestamp_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        *pTimeStamp);

/* Function Name:
 *      rtk_time_portRefTime_get
 * Description:
 *      Get the reference time of the specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id, it should be base port of PHY
 * Output:
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          Change name from rtk_time_refTime_get.
 *          Add port parameter.
 */
extern int32
rtk_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      rtk_time_portRefTime_set
 * Description:
 *      Set the reference time of the specified portmask.
 * Input:
 *      unit      - unit id
 *      portmask    - portmask, it should be base ports of PHYs
 *      timeStamp - reference timestamp value
 *      exec      - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          Change name from rtk_time_refTime_set.
 *          Add portmask parameter.
 */
extern int32
rtk_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      rtk_time_portRefTimeAdjust_set
 * Description:
 *      Adjust the reference time of portmask.
 * Input:
 *      unit      - unit id
 *      portmask    - portmask, it should be base ports of PHYs
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec      - 0 : do not execute, 1: execute
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 * Changes:
 *      [3.2.0]
 *          Change name from rtk_time_refTimeAdjust_set.
 *          Add portmask parameter.
 */
extern int32
rtk_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      rtk_time_portRefTimeEnable_get
 * Description:
 *      Get the enable state of reference time of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id, it should be base port of PHY
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          Change name from rtk_time_refTimeEnable_get.
 *          Add port parameter.
 */
extern int32
rtk_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_portRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of the specified portmask.
 * Input:
 *      unit   - unit id
 *      portmask    - portmask, it should be base ports of PHYs
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          Change name from rtk_time_refTimeEnable_set.
 *          Add portmask parameter.
 */
extern int32
rtk_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_portRefTimeFreq_get
 * Description:
 *      Get the frequency of PTP reference time of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id, it should be base port of PHY
 * Output:
 *      pFreq  - pointer to reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x80000000.
 *      If it is configured to 0x40000000, the tick frequency would be half of default.
 *      If it is configured to 0xC0000000, the tick frequency would be one and half times of default.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *      [3.2.0]
 *          Change name from rtk_time_refTimeFreq_get.
 *          Add port parameter.
 */
extern int32
rtk_time_portRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq);


/* Function Name:
 *      rtk_time_portRefTimeFreq_set
 * Description:
 *      Set the frequency of PTP reference time of the specified portmask.
 * Input:
 *      unit   - unit id
 *      portmask    - portmask, it should be base ports of PHYs
 *      freq   - reference time frequency
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      The frequency configuration decides the reference time tick frequency.
 *      The default value is 0x80000000.
 *      If it is configured to 0x40000000, the tick frequence would be half of default.
 *      If it is configured to 0xC0000000, the tick frequence would be one and half times of default.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *      [3.2.0]
 *          Change name from rtk_time_refTimeFreq_set.
 *          Add portmask parameter.
 */
extern int32
rtk_time_portRefTimeFreq_set(uint32 unit, rtk_portmask_t portmask, uint32 freq);

/* Function Name:
 *      rtk_time_correctionFieldTransparentValue_get
 * Description:
 *      Get the transparent value of correction field for transparent clock.
 * Input:
 *      unit   - unit id
 *      oriCf   - original correction field value
 *      rxTimeStamp   - RX time stamp of the packet
 * Output:
 *      pTransCf - pointer to transparent correction field value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      This API calculate the value of transparent correction field by original correction field - rxTimeStamp.
 *      Software should replace the correction field in packet with the transparent correction field and send the packet.
 *      Hardware would replace the correction field by the result of TX TimeStamp - RX TimeStamp + original correction field.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_time_correctionFieldTransparentValue_get(uint32 unit, int64 oriCf, rtk_time_timeStamp_t rxTimeStamp, int64 *pTransCf);

/* Function Name:
 *      rtk_time_portPtpMacAddr_get
 * Description:
 *      Get the PTP MAC address of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pMac  - pointer to MAC adress
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pMac);

/* Function Name:
 *      rtk_time_portPtpMacAddr_set
 * Description:
 *      Set the PTP MAC address of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port ID
 *      pMac  - pointer to MAC adress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pMac);

/* Function Name:
 *      rtk_time_portPtpMacAddrRange_get
 * Description:
 *      Get the PTP MAC address of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 * Output:
 *      pMacRange  - pointer to MAC adress range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpMacAddrRange_get(uint32 unit, rtk_port_t port, uint32 *pRange);

/* Function Name:
 *      rtk_time_portPtpMacAddrRange_set
 * Description:
 *      Set the PTP MAC address range of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port ID
 *      range  - pointer to MAC adress range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpMacAddrRange_set(uint32 unit, rtk_port_t port, uint32 range);

/* Function Name:
 *      rtk_time_portPtpVlanTpid_get
 * Description:
 *      Get the VLAN TPID of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      type   - outer or inner VLAN
 *      idx    - TPID entry index
 * Output:
 *      pTpid  - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpVlanTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 idx, uint32 *pTpid);

/* Function Name:
 *      rtk_time_portPtpVlanTpid_set
 * Description:
 *      Set the VLAN TPID of specific port.
 * Input:
 *      unit    - unit id
 *      port   - port ID
 *      type   - outer or inner VLAN
 *      idx     - TPID entry index
 *      tpid    - VLAN TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpVlanTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 idx, uint32 tpid);

/* Function Name:
 *      rtk_time_portPtpOper_get
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
 * Applicable:
 *      8390, 8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

/* Function Name:
 *      rtk_time_portPtpOper_set
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
 * Applicable:
 *      8390, 8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

/* Function Name:
 *      rtk_time_portPtpLatchTime_get
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
 * Applicable:
 *      8390, 8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.0]
 *          New added function.
 */
extern int32
rtk_time_portPtpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime);

/* Function Name:
 *      rtk_time_portPtpRefTimeFreqCfg_get
 * Description:
 *      Get the frequency of reference time of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pFreqCfg    - pointer to configured reference time frequency
 *      pFreqCur    - pointer to current reference time frequency
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpRefTimeFreqCfg_get(uint32 unit, rtk_port_t port, uint32 *pFreqCfg, uint32 *pFreqCur);

/* Function Name:
 *      rtk_time_portPtpRefTimeFreqCfg_set
 * Description:
 *      Set the frequency of reference time of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      freq        - reference time frequency
 *      apply       - if the frequency is applied immediately
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpRefTimeFreqCfg_set(uint32 unit, rtk_port_t port, uint32 freq, uint32 apply);

/* Function Name:
 *      rtk_time_portPtpTxInterruptStatus_get
 * Description:
 *      Get the TX timestamp FIFO non-empty interrupt status of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *
 * Output:
 *      pIntrSts    - interrupt status of RX/TX PTP frame types
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpTxInterruptStatus_get(uint32 unit, rtk_port_t port, uint32 *pIntrSts);

/* Function Name:
 *      rtk_time_portPtpInterruptEnable_get
 * Description:
 *      Get the PTP TX timestamp FIFO non-empty interrupt enable status of the specified port.
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
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpInterruptEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_portPtpInterruptEnable_set
 * Description:
 *      Set the PTP TX timestamp FIFO non-empty interrupt enable status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpInterruptEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_portPtpTxTimestampFifo_get
 * Description:
 *      Get the top entry from PTP Tx timstamp FIFO on the dedicated port from the specified device. of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pTimeEntry  - pointer buffer of TIME timestamp entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpTxTimestampFifo_get(uint32 unit, rtk_port_t port, rtk_time_txTimeEntry_t *pTimeEntry);

/* Function Name:
 *      rtk_time_portPtp1PPSOutput_get
 * Description:
 *      Get the 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPulseWidth - pointer to 1 PPS pulse width
 *      pEnable     - pointer to 1 PPS output enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtp1PPSOutput_get(uint32 unit, rtk_port_t port, uint32 *pPulseWidth, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_portPtp1PPSOutput_set
 * Description:
 *      Set the 1 PPS output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pulseWidth  - pointer to 1 PPS pulse width
 *      enable      - enable 1 PPS output
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtp1PPSOutput_set(uint32 unit, rtk_port_t port, uint32 pulseWidth, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_portPtpClockOutput_get
 * Description:
 *      Get the clock output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pClkOutput  - pointer to clock output configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpClockOutput_get(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput);

/* Function Name:
 *      rtk_time_portPtpClockOutput_set
 * Description:
 *      Set the clock output configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      pClkOutput  - pointer to clock output configuration
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpClockOutput_set(uint32 unit, rtk_port_t port, rtk_time_clkOutput_t *pClkOutput);

/* Function Name:
 *      rtk_time_portPtpOutputSigSel_get
 * Description:
 *      Get the output pin signal selection configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pOutSigSel  - pointer to output pin signal selection configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpOutputSigSel_get(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t *pOutSigSel);

/* Function Name:
 *      rtk_time_portPtpOutputSigSel_set
 * Description:
 *      Set the output pin signal selection configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      outSigSel   - output pin signal selection configuration
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpOutputSigSel_set(uint32 unit, rtk_port_t port, rtk_time_outSigSel_t outSigSel);

/* Function Name:
 *      rtk_time_portPtpTransEnable_get
 * Description:
 *      Get the enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pEnable     - pointer to PTP transparent clock enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpTransEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_portPtpTransEnable_set
 * Description:
 *      Set the enable status for PTP transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      enable      - PTP transparent clock enable status
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpTransEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_portPtpLinkDelay_get
 * Description:
 *      Get the link delay for PTP p2p transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pLinkDelay  - pointer to link delay (unit: nsec)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpLinkDelay_get(uint32 unit, rtk_port_t port, uint32 *pLinkDelay);

/* Function Name:
 *      rtk_time_portPtpLinkDelay_set
 * Description:
 *      Set the link delay for PTP p2p transparent clock of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      linkDelay   - link delay (unit: nsec)
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
 *      8218E
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.3]
 *          New added function.
 */
extern int32
rtk_time_portPtpLinkDelay_set(uint32 unit, rtk_port_t port, uint32 linkDelay);

/* Function Name:
 *      rtk_port_macsecReg_get
 * Description:
 *      Get PHY MACSec register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - macsec reg type
 *      reg     - register address
 * Output:
 *      pData   - pointer to the reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8261N, 8261B, 8264B, 8264b
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_port_macsecReg_get(uint32 unit, rtk_port_t port, uint32 type, uint32 reg,
    uint32 *pData);

/* Function Name:
 *      rtk_port_macsecReg_set
 * Description:
 *      Set PHY MACSec register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - macsec reg type
 *      reg     - register address
 *      data    - value write to register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8261N, 8261B, 8264B, 8264b
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_port_macsecReg_set(uint32 unit, rtk_port_t port, uint32 type, uint32 reg,
    uint32 data);

/* Function Name:
 *      rtk_macsec_port_cfg_set
 * Description:
 *      Set per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      pPortcfg - pointer to macsec port configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_port_cfg_set(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      rtk_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_port_cfg_get(uint32 unit, rtk_port_t port,
    rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      rtk_macsec_sc_create
 * Description:
 *      Create a MACsec Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      pSc      - pointer to macsec sc configuration structure
 * Output:
 *      pSc_id   - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sc_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    rtk_macsec_sc_t *pSc, uint32 *pSc_id);

/* Function Name:
 *      rtk_macsec_sc_get
 * Description:
 *      Get configuration info for a created Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_index - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sc_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_t *pSc);

/* Function Name:
 *      rtk_macsec_sc_del
 * Description:
 *      Delete a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sc_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id);

/* Function Name:
 *      rtk_macsec_sc_status_get
 * Description:
 *      Get hardware status for a Secure Channel
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_status - pointer to macsec SC status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sc_status_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_sc_status_t *pSc_status);

/* Function Name:
 *      rtk_macsec_sa_create
 * Description:
 *      Create a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 *      pSa      - pointer to macsec SA configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sa_create(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);

/* Function Name:
 *      rtk_macsec_sa_get
 * Description:
 *      Get configuration info for a Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      pSa      - pointer to macsec SA configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sa_get(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);

/* Function Name:
 *      rtk_macsec_sa_del
 * Description:
 *      Delete a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sa_del(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      rtk_macsec_sa_activate
 * Description:
 *      Activate a MACsec Secure Association
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - Secure Channel id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      For egress, this function will change running SA.
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_sa_activate(uint32 unit, rtk_port_t port, rtk_macsec_dir_t dir,
    uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      rtk_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association (inUse = 0)
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_rxsa_disable(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an);

/* Function Name:
 *      rtk_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association (inUse = 0)
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_txsa_disable(uint32 unit, rtk_port_t port, uint32 txsc_id);

/* Function Name:
 *      rtk_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_stat_clear(uint32 unit, rtk_port_t port);

/* Function Name:
 *      rtk_macsec_stat_port_get
 * Description:
 *      get per-port statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_stat_port_get(uint32 unit, rtk_port_t port, rtk_macsec_stat_t stat,
    uint64 *pCnt);

/* Function Name:
 *      rtk_macsec_stat_txsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      txsc_id  - egress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_stat_txsa_get(uint32 unit, rtk_port_t port, uint32 txsc_id,
    rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, uint64 *pCnt);

/* Function Name:
 *      rtk_macsec_stat_rxsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_stat_rxsa_get(uint32 unit, rtk_port_t port, uint32 rxsc_id,
    rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, uint64 *pCnt);

/* Function Name:
 *      rtk_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8261N, 8261B, 8264B
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_macsec_intr_status_get(uint32 unit, rtk_port_t port,
    rtk_macsec_intr_status_t *pIntr_status);

/* Function Name:
 *      rtk_port_phySdsReg_get
 * Description:
 *      Get PHY SerDes register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - SerDes device address
 *      sdsReg  - SerDes reg id
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
 *      8224
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_port_phySdsReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              sdsPage,
    uint32              sdsReg,
    uint32              *pData);

/* Function Name:
 *      rtk_port_phySdsReg_set
 * Description:
 *      Set PHY SerDes register data of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsPage - SerDes device address
 *      sdsReg  - SerDes reg id
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
 *      8224
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.9]
 *          New added function.
 */
extern int32
rtk_port_phySdsReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              sdsPage,
    uint32              sdsReg,
    uint32              data);

/* Function Name:
 *      rtk_time_portPtpFullSec_get
 * Description:
 *      Get the full second from a partial second information of a specific device
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      partialSec  - partial second
 *      bitLen      - partial second bit length
 * Output:
 *      pFullSec    - pointer to 48 bits full second
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      826XB, 8261N
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 */
extern int32
rtk_time_portPtpFullSec_get(uint32 unit, rtk_port_t port, uint64 partialSec,
    uint32 bitLen, uint64 *pFullSec);
#endif /* __RTK_PHY_H__ */

