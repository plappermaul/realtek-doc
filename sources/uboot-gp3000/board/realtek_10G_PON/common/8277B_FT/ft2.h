#ifndef _FT2_H
#define _FT2_H

#define CHIP_ID(x)              ((x >> 12) & 0xFFFF)

#define GLOBAL_REG_BASE         GLOBAL_JTAG_ID

#define GLOBAL_FUSE_BASE        GLOBAL_FUSE_CONTROL

#define BL2_TEST_RLT    GLOBAL_SOFTWARE2
/* Bitfield for GLOBAL_SOFTWARE2 */
#define BL2_TEST_RTC    0x00000001
#define BL2_TEST_OTP    0x00000002
#define BL2_TEST_DDR_R480_ZQ 0x00000010
#define BL2_TEST_DDR_CMD_ZQ  0x00000020
#define BL2_TEST_DDR_DQ_ZQ   0x00000040


#define MEM_BIT_MODE    0x01111004
#define MEM_DDR_TYPE    0x01111008
#define MEM_DDR_SIZE    0x0111100C
#define MEM_DDR_FREQ    0x01111010

#define I2C_MUX_CHANNEL         0
#define HWMON_ENTRY                     1
#define INPUT_MUX_ENTRY         2
#define REVERT_ENTRY            3
#define INPUT_VAL_SIZE          8

#define PHY_REG_MAX_LEN         16
#define NONE_PORT                       0xfe
#define INVALID_PORT            0xff
#define PHY_CHANNEL_NUM         4
#define PHY_ADO_CAL_TIMES       10
#define PHY_RC_K_TIMES          10
#define PHY_R_K_TIMES           10
#define PHY_ADJ_TIMEOUT         16

#define PHY_READY_TIMEOUT       0xffff

typedef enum {
    CA_PORT_ID_NI0 = 0,     /* 1 Gbps NI Port (Integrated GPHY) */
    CA_PORT_ID_NI1 = 1,     /* 1 Gbps NI Port (Integrated GPHY) */
    CA_PORT_ID_NI2 = 2,     /* 1 Gbps NI Port (Integrated GPHY) */
    CA_PORT_ID_NI3 = 3,     /* 1 Gbps NI Port (Integrated GPHY) */
    CA_PORT_ID_NI4 = 4,     /* 1 Gbps NI Port (RGMII)*/
#ifndef CONFIG_ARCH_CORTINA_G3LITE
    CA_PORT_ID_NI5 = 5,     /* 10 Gbps XFI NI Port (RXAUI/2.5GSGMII/RGMII) */
    CA_PORT_ID_NI6 = 6,     /* 10 Gbps XFI NI Port (LAN) */
    CA_PORT_ID_NI7 = 7,     /* 10 Gbps XFI NI Port (WAN) */
#endif
#ifdef CONFIG_ARCH_CORTINA_G3LITE/*CODEGEN_IGNORE_TAG*/
    CA_PORT_ID_NI5 = CA_PORT_INDEX_INVALID,
    CA_PORT_ID_NI6 = CA_PORT_INDEX_INVALID,
    CA_PORT_ID_NI7 = CA_PORT_INDEX_INVALID,
#endif
    CA_PORT_ID_DEEPQ0 = 0,
    CA_PORT_ID_DEEPQ1 = 1,
    CA_PORT_ID_DEEPQ2 = 2,
    CA_PORT_ID_DEEPQ3 = 3,
    CA_PORT_ID_DEEPQ4 = 4,
#ifndef CONFIG_ARCH_CORTINA_G3LITE
    CA_PORT_ID_DEEPQ5 = 5,
    CA_PORT_ID_DEEPQ6 = 6,
    CA_PORT_ID_DEEPQ7 = 7,
#endif
#ifdef CONFIG_ARCH_CORTINA_G3LITE/*CODEGEN_IGNORE_TAG*/
    CA_PORT_ID_DEEPQ5 = CA_PORT_INDEX_INVALID,
    CA_PORT_ID_DEEPQ6 = CA_PORT_INDEX_INVALID,
    CA_PORT_ID_DEEPQ7 = CA_PORT_INDEX_INVALID,
#endif
    CA_PORT_ID_CPU0 = 16,
    CA_PORT_ID_CPU1 = 17,
    CA_PORT_ID_CPU2 = 18,
    CA_PORT_ID_CPU3 = 19,
    CA_PORT_ID_CPU4 = 20,
    CA_PORT_ID_CPU5 = 21,
    CA_PORT_ID_CPU6 = 22,
    CA_PORT_ID_CPU7 = 23,

    CA_PORT_ID_L3_WAN = 24,        /* port on L3 (interface) */
    CA_PORT_ID_L3_LAN = 25,        /* port on L3 (interface) */

    CA_PORT_ID_OAM = 26,        /* oam port */
    CA_PORT_ID_MC = 27,         /* multicast port */
    CA_PORT_ID_CPU_Q = 29,      /* cpu port */
    CA_PORT_ID_SPCL = 30,       /* ldpid for special packet resolution */
    CA_PORT_ID_BLACKHOLE = 31,
    CA_PORT_ID_CPU_MQ_MIN = 32,
    CA_PORT_ID_CPU_MQ_MAX = 63,

} ca_port_index_t;

/* Ethernet Rx MIB item enumeration */
typedef enum
{
        __NI_MIB_RX_UC_PKT_CNT              = 0x0 ,
        __NI_MIB_RX_MC_FRM_CNT              = 0x1 ,
        __NI_MIB_RX_BC_FRM_CNT              = 0x2 ,
        __NI_MIB_RX_OAM_FRM_CNT             = 0x3 ,
        __NI_MIB_RX_JUMBO_FRM_CNT           = 0x4 ,
        __NI_MIB_RX_PAUSE_FRM_CNT           = 0x5 ,
        __NI_MIB_RX_PFC_FRM_CNT             = 0x6 ,
        __NI_MIB_RX_UNK_OPCODE_FRM_CNT      = 0x7 ,
        __NI_MIB_RX_CRC_ERR_FRM_CNT         = 0x8 ,
        __NI_MIB_RX_UNDER_SIZE_FRM_CNT      = 0x9 ,
        __NI_MIB_RX_RUNT_FRM_CNT            = 0xA ,
        __NI_MIB_RX_OVSIZE_FRM_CNT          = 0xB ,
        __NI_MIB_RX_JABBER_FRM_CNT          = 0xC ,
        __NI_MIB_RX_INVALID_FRM_CNT         = 0xD ,
        __NI_MIB_RX_INVALID_FLOW_FRM_CNT    = 0xE ,
        __NI_MIB_RX_STATS_FRM_64_OCT        = 0xF ,
        __NI_MIB_RX_STATS_FRM_65_TO_127_OCT    = 0x10 ,
        __NI_MIB_RX_STATS_FRM_128_TO_255_OCT   = 0x11 ,
        __NI_MIB_RX_STATS_FRM_256_TO_511_OCT   = 0x12,
        __NI_MIB_RX_STATS_FRM_512_TO_1023_OCT  = 0x13,
        __NI_MIB_RX_STATS_FRM_1024_TO_1518_OCT = 0x14,
        __NI_MIB_RX_STATS_FRM_1519_TO_2100_OCT = 0x15,
        __NI_MIB_RX_STATS_FRM_2101_TO_9200_OCT = 0x16,
        __NI_MIB_RX_STATS_FRM_9201_TO_MAX_OCT  = 0x17,
        __NI_MIB_RX_BYTE_COUNT_LO              = 0x18,
        __NI_MIB_RX_BYTE_COUNT_HI              = 0x19,

        __NI_MIB_RX_NUM
} __ni_rx_mib_id_t;

/* Ethernet Tx MIB item enumeration */
typedef enum
{
        __NI_MIB_TX_UC_PKT_CNT                 = 0x0 ,
        __NI_MIB_TX_MC_FRM_CNT                 = 0x1 ,
        __NI_MIB_TX_BC_FRM_CNT                 = 0x2 ,
        __NI_MIB_TX_OAM_FRM_CNT                = 0x3 ,
        __NI_MIB_TX_JUMBO_FRM_CNT              = 0x4 ,
        __NI_MIB_TX_PAUSE_FRM_CNT              = 0x5 ,
        __NI_MIB_TX_PFC_FRM_CNT                = 0x6 ,
        __NI_MIB_TX_CRCERR_FRM_CNT             = 0x7 ,
        __NI_MIB_TX_OVSIZE_FRM_CNT             = 0x8 ,
        __NI_MIB_TX_SINGLE_COL_FRM_CNT         = 0x9 ,
        __NI_MIB_TX_MULTI_COL_FRM_CNT          = 0xA ,
        __NI_MIB_TX_LATE_COL_FRM_CNT           = 0xB ,
        __NI_MIB_TX_RSVD_12, /* 0xC is reserved */
        __NI_MIB_TX_EXESS_COL_FRM_CNT          = 0xD ,
        __NI_MIB_TX_STATS_FRM_64_OCT           = 0xE ,
        __NI_MIB_TX_STATS_FRM_65_TO_127_OCT    = 0xF ,
        __NI_MIB_TX_STATS_FRM_128_TO_255_OCT   = 0x10 ,
        __NI_MIB_TX_STATS_FRM_256_TO_511_OCT   = 0x11,
        __NI_MIB_TX_STATS_FRM_512_TO_1023_OCT  = 0x12,
        __NI_MIB_TX_STATS_FRM_1024_TO_1518_OCT = 0x13,
        __NI_MIB_TX_STATS_FRM_1519_TO_2100_OCT = 0x14,
        __NI_MIB_TX_STATS_FRM_2101_TO_9200_OCT = 0x15,
        __NI_MIB_TX_STATS_FRM_9201_TO_MAX_OCT  = 0x16,
        __NI_MIB_TX_RSVD_23 = 0x17,/* 0x17 is reserved */
        __NI_MIB_TX_BYTE_CNT_LO                = 0x18,
        __NI_MIB_TX_BYTE_CNT_HI                = 0x19,

        __NI_MIB_TX_NUM

} __ni_tx_mib_id_t;


typedef enum
{
        FT2_ERROR       = -1,
        FT2_TIMEOUT     = -2,
        FT2_MDIO        = -3,
        FT2_GPIO        = -4,
        FT2_I2C         = -5,
        FT2_CFG = -6,
        FT2_RTC         = -7,
        FT2_PCIE        = -8,
        FT2_USB         = -9,
        FT2_SATA        = -10,
        FT2_CPU         = -11,
        FT2_OTP         = -12,
        FT2_TAROKO      = -13,
        FT2_SERDES = -14,
        FT2_PON_BEN = -15,
        FT2_PON_SRAM = -16,
        FT2_LDMA = -17,
        FT2_UUID = -18,
        FT2_SSP         = -19,
        FT2_DYINGASP    = -20,
        FT2_DDR_ZQ      = -21,
        FT2_NAND        = -22,
        FT2_GPHY_SNR    = -23,
        FT2_OK          = 0
} ft2_ret_t;

typedef enum
{
        LAN_SWITCH_BUS0 = 0,
        LAN_SWITCH_BUS1 = 1,
        LAN_SWITCH_OPEN = 2
} lan_switch_t;

typedef enum {
        PHY_CHANNEL_A = 0,
        PHY_CHANNEL_B = 1,
        PHY_CHANNEL_C = 2,
        PHY_CHANNEL_D = 3,
        PHY_CHANNEL_MAX
} phy_channel_t;

typedef enum
{
        PHY_ADO_CURR_NORM = 0,
        PHY_ADO_CURR_LOW = 1,
        PHY_ADO_CURR_MED = 2,
        PHY_ADO_CURR_HIGH = 3,
        PHY_ADO_CURR_MAX
} phy_ado_current_t;

typedef enum {
        PHY_K_ADO = 0,
        PHY_K_RC_LEN,
        PHY_K_RC_RLEN,
        PHY_K_R_TAPBIN_TX,
        PHY_K_R_TAPBINRX_PM,
        PHY_K_AMP_IBADJ,
        PHY_K_MDI,
        PHY_K_MAX
} phy_k_item_t;


typedef enum {
        FT2_PARAM_GPHY_K = 0,
        FT2_PARAM_RC_K_DIFF,
        FT2_PARAM_R_K_DIFF,
        FT2_PARAM_AMP_AID,
        FT2_PARAM_XFI_PORT_LOOPBACK,
        FT2_PARAM_GMII_PORT_LOOPBACK,
        FT2_PARAM_GMII_PORT_OPEN_LOOPBACK,
        FT2_PARAM_RGMII_PORT_LOOPBACK,
        FT2_PARAM_SERDES,
        FT2_PARAM_PON_BEN,
        FT2_PARAM_PON_SRAM,
        FT2_PARAM_INTRA_XFI,
        FT2_PARAM_CPU,
        FT2_PARAM_PCIE,
        FT2_PARAM_USB,
        FT2_PARAM_SATA,
        FT2_PARAM_RTC,
        FT2_PARAM_OTP,
        FT2_PARAM_TAROKO,
        FT2_PARAM_GPIO,
        FT2_PARAM_LDMA,
        FT2_PARAM_DDR_ZQ,
        FT2_PARAM_UUID,
        FT2_PARAM_NAND,
        FT2_PARAM_CFG_W,
        FT2_PARAM_SSP,
        FT2_PARAM_DYINGASP,
        FT2_PARAM_GPHY_SNR,
        FT2_PARAM_DBG,
        FT2_PARAM_MAX
} ft2_param_type_t;

typedef struct serdes_cfg_s {
        uint32_t addr;
        uint32_t value;
} serdes_cfg_t;


/******************************************************************************/
#define MODULE_MAX_ENTRY        8

typedef struct ft2_record_s {
        char *name;
        void *ptr_data;

        uint32_t module_rlt;
        uint32_t entry_mask;
        char *entry_name[MODULE_MAX_ENTRY];
        uint32_t entry_test[MODULE_MAX_ENTRY];
        uint32_t entry_rlt[MODULE_MAX_ENTRY];

        unsigned long start;
        unsigned long finish; /* msecs */

        uint32_t data;
} ft2_record_t;

/*******************************************************************************
 * GPHY Calibration
 ******************************************************************************/
#define TEST_ADO_CALIBRATION    (1 << 0)
#define TEST_RC_CALIBRATION             (1 << 1)
#define TEST_R_CALIBRATION              (1 << 2)
#define TEST_AMP_CALIBRATION    (1 << 3)
#define TEST_MDI_CALIBRATION    (1 << 4)
#define TEST_GPHY_SNR                   (1 << 5)
#define TEST_ALL_CALIBRATION    (TEST_ADO_CALIBRATION | TEST_RC_CALIBRATION | \
                                 TEST_R_CALIBRATION | TEST_AMP_CALIBRATION | \
                                 TEST_MDI_CALIBRATION)


/*******************************************************************************
 * CFG Operation
 ******************************************************************************/
#define FT2_CFG_GALIB_LOAD      0xFF
#define FT2_CFG_GALIB_CHECK     0xFFF
#define FT2_CFG_GALIB_WRITE     0xFFFF

/*******************************************************************************
 * Network Interface
 ******************************************************************************/
#define E_SERDES_EPON   (1 << 0)
#define E_SERDES_GPON   (1 << 1)
#define E_SERDES_I2C    (1 << 2)
#define TEST_SERDES     (E_SERDES_EPON | E_SERDES_GPON | E_SERDES_I2C)

/*******************************************************************************
 * Misc.
 ******************************************************************************/
#define E_PON_SRAM_DN_NANDI_SADB                (1 << 0)
#define E_PON_SRAM_UP_NANDI_SADB                (1 << 1)
#define E_PON_SRAM_DN_TX_RX_MIB         (1 << 2)
#define E_PON_SRAM_UP_TX_RX_MIB         (1 << 3)
#define E_PON_SRAM_AES_DEC_KEY          (1 << 4)
#define TEST_PON_SRAM           (E_PON_SRAM_DN_NANDI_SADB | E_PON_SRAM_UP_NANDI_SADB | E_PON_SRAM_UP_TX_RX_MIB | E_PON_SRAM_AES_DEC_KEY)

#define TEST_OTP_CHECK          (1 << 0)

#define TEST_DDR_R480_ZQ                (1 << 0)
#define TEST_DDR_CMD_ZQ         (1 << 1)
#define TEST_DDR_DQ_ZQ          (1 << 2)

#define TEST_RTC_ACCESS         (1 << 0)
#define TEST_RTC_TIME                   (1 << 1)
#define RTC_SPEEDUP_SCALE               4096
#define RTC_TIME_TOLERANCE              2

#define E_USB_LINK      (1 << 0)
#define E_USB_GEN       (1 << 1)
#define E_USB_MANU (1 << 2)
#define TEST_USB_LINK   (E_USB_LINK | E_USB_GEN | E_USB_MANU)

#define TEST_TAROKO_LOAD                (1 << 0)
#define TEST_TAROKO_RUN         (1 << 1)

#define E_PCIE_LINK     (1 << 0)
#define E_PCIE_GEN      (1 << 1)
#define E_PCIE_LANE     (1 << 2)
#define E_PCIE_VENDOR   (1 << 5)
#define TEST_PCIE_LINK  (E_PCIE_LINK | E_PCIE_GEN | E_PCIE_LANE | E_PCIE_VENDOR)


#endif
