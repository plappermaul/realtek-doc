#ifndef _RTK_PCIE_8852A_
#define _RTK_PCIE_8852A_

// temporary register define
#define R_AX_HIMR0                  0x01a0
#define B_AX_TXDMA_CH12_INT_EN      BIT(13)
#define B_AX_TXDMA_ACH1_INT_EN      BIT(4)
#define B_AX_TXDMA_ACH0_INT_EN      BIT(3)
#define B_AX_RPQDMA_INT_EN          BIT(2)
#define B_AX_RXDMA_INT_EN           BIT(0)

#define R_AX_HISR0                  0x01a4
#define B_AX_TXDMA_CH12_INT         BIT(13)
#define B_AX_TXDMA_ACH1_INT         BIT(4)
#define B_AX_TXDMA_ACH0_INT         BIT(3)
#define B_AX_RPQDMA_INT             BIT(2)
#define B_AX_RXDMA_INT              BIT(0)

#define R_AX_PCIE_INIT_CFG1         0x1000
#define B_AX_PCIE_RST_TRXDMA_INTF   BIT(19)
#define B_AX_RXBD_MODE              BIT(18)
#define B_AX_PCIE_MAX_RXDMA_SH      14
#define B_AX_PCIE_MAX_RXDMA_MSK     0x7
#define B_AX_RXHCI_EN               BIT(13)
#define B_AX_LATENCY_CONTROL        BIT(12)
#define B_AX_TXHCI_EN               BIT(11)
#define B_AX_PCIE_MAX_TXDMA_SH      8
#define B_AX_PCIE_MAX_TXDMA_MSK     0x7
#define B_AX_EN_ALIGN_MTU           BIT(6)
#define B_AX_TX_TRUNC_MODE          BIT(5)
#define B_AX_RX_TRUNC_MODE          BIT(4)
#define B_AX_DIS_RXDMA_PRE          BIT(2)

#define R_AX_PCIE_INIT_CFG2         0x1004
#define B_AX_PCIE_RX_APPLEN_SH      0
#define B_AX_PCIE_RX_APPLEN_MSK     0x3fff

#define R_AX_PCIE_DMA_STOP1         0x1010
#define B_AX_STOP_WPDMA             BIT(19)
#define B_AX_STOP_CH12              BIT(18)
#define B_AX_STOP_CH9               BIT(17)
#define B_AX_STOP_CH8               BIT(16)
#define B_AX_STOP_ACH7              BIT(15)
#define B_AX_STOP_ACH6              BIT(14)
#define B_AX_STOP_ACH5              BIT(13)
#define B_AX_STOP_ACH4              BIT(12)
#define B_AX_STOP_ACH3              BIT(11)
#define B_AX_STOP_ACH2              BIT(10)
#define B_AX_STOP_ACH1              BIT(9)
#define B_AX_STOP_ACH0              BIT(8)

#define R_AX_RXQ_RXBD_NUM           0x1020
#define B_AX_RXQ_DESC_NUM_SH        0
#define B_AX_RXQ_DESC_NUM_MSK       0xfff

#define R_AX_RPQ_RXBD_NUM           0x1022
#define B_AX_RPQ_DESC_NUM_SH        0
#define B_AX_RPQ_DESC_NUM_MSK       0xfff

#define R_AX_ACH0_TXBD_NUM          0x1024
#define B_AX_PCIE_ACH0_FLAG         BIT(14)
#define B_AX_ACH0_DESC_NUM_SH       0
#define B_AX_ACH0_DESC_NUM_MSK      0xfff

#define R_AX_ACH1_TXBD_NUM          0x1026
#define B_AX_PCIE_ACH1_FLAG         BIT(14)
#define B_AX_ACH1_DESC_NUM_SH       0
#define B_AX_ACH1_DESC_NUM_MSK      0xfff

#define R_AX_ACH2_TXBD_NUM          0x1028
#define B_AX_PCIE_ACH2_FLAG         BIT(14)
#define B_AX_ACH2_DESC_NUM_SH       0
#define B_AX_ACH2_DESC_NUM_MSK      0xfff

#define R_AX_ACH3_TXBD_NUM          0x102A
#define B_AX_PCIE_ACH3_FLAG         BIT(14)
#define B_AX_ACH3_DESC_NUM_SH       0
#define B_AX_ACH3_DESC_NUM_MSK      0xfff

#define R_AX_ACH4_TXBD_NUM          0x102C
#define B_AX_PCIE_ACH4_FLAG         BIT(14)
#define B_AX_ACH4_DESC_NUM_SH       0
#define B_AX_ACH4_DESC_NUM_MSK      0xfff

#define R_AX_ACH5_TXBD_NUM          0x102E
#define B_AX_PCIE_ACH5_FLAG         BIT(14)
#define B_AX_ACH5_DESC_NUM_SH       0
#define B_AX_ACH5_DESC_NUM_MSK      0xfff

#define R_AX_ACH6_TXBD_NUM          0x1030
#define B_AX_PCIE_ACH6_FLAG         BIT(14)
#define B_AX_ACH6_DESC_NUM_SH       0
#define B_AX_ACH6_DESC_NUM_MSK      0xfff

#define R_AX_ACH7_TXBD_NUM          0x1032
#define B_AX_PCIE_ACH7_FLAG         BIT(14)
#define B_AX_ACH7_DESC_NUM_SH       0
#define B_AX_ACH7_DESC_NUM_MSK      0xfff

#define R_AX_CH8_TXBD_NUM           0x1034
#define B_AX_PCIE_CH8_FLAG          BIT(14)
#define B_AX_CH8_DESC_NUM_SH        0
#define B_AX_CH8_DESC_NUM_MSK       0xfff

#define R_AX_CH9_TXBD_NUM           0x1036
#define B_AX_PCIE_CH9_FLAG          BIT(14)
#define B_AX_CH9_DESC_NUM_SH        0
#define B_AX_CH9_DESC_NUM_MSK       0xfff

#define R_AX_CH12_TXBD_NUM          0x1038
#define B_AX_PCIE_CH12_FLAG         BIT(14)
#define B_AX_CH12_DESC_NUM_SH       0
#define B_AX_CH12_DESC_NUM_MSK      0xfff

#define R_AX_RXQ_RXBD_IDX           0x1050
#define B_AX_RXQ_HW_IDX_SH          16
#define B_AX_RXQ_HW_IDX_MSK         0xfff
#define B_AX_RXQ_HOST_IDX_SH        0
#define B_AX_RXQ_HOST_IDX_MSK       0xfff

#define R_AX_RPQ_RXBD_IDX           0x1054
#define B_AX_RPQ_HW_IDX_SH          16
#define B_AX_RPQ_HW_IDX_MSK         0xfff
#define B_AX_RPQ_HOST_IDX_SH        0
#define B_AX_RPQ_HOST_IDX_MSK       0xfff

#define R_AX_ACH0_TXBD_IDX          0x1058
#define B_AX_ACH0_HW_IDX_SH         16
#define B_AX_ACH0_HW_IDX_MSK        0xfff
#define B_AX_ACH0_HOST_IDX_SH       0
#define B_AX_ACH0_HOST_IDX_MSK      0xfff

#define R_AX_ACH1_TXBD_IDX          0x105C
#define B_AX_ACH1_HW_IDX_SH         16
#define B_AX_ACH1_HW_IDX_MSK        0xfff
#define B_AX_ACH1_HOST_IDX_SH       0
#define B_AX_ACH1_HOST_IDX_MSK      0xfff

#define R_AX_ACH2_TXBD_IDX          0x1060
#define B_AX_ACH2_HW_IDX_SH         16
#define B_AX_ACH2_HW_IDX_MSK        0xfff
#define B_AX_ACH2_HOST_IDX_SH       0
#define B_AX_ACH2_HOST_IDX_MSK      0xfff

#define R_AX_ACH3_TXBD_IDX          0x1064
#define B_AX_ACH3_HW_IDX_SH         16
#define B_AX_ACH3_HW_IDX_MSK        0xfff
#define B_AX_ACH3_HOST_IDX_SH       0
#define B_AX_ACH3_HOST_IDX_MSK      0xfff

#define R_AX_ACH4_TXBD_IDX          0x1068
#define B_AX_ACH4_HW_IDX_SH         16
#define B_AX_ACH4_HW_IDX_MSK        0xfff
#define B_AX_ACH4_HOST_IDX_SH       0
#define B_AX_ACH4_HOST_IDX_MSK      0xfff

#define R_AX_ACH5_TXBD_IDX          0x106C
#define B_AX_ACH5_HW_IDX_SH         16
#define B_AX_ACH5_HW_IDX_MSK        0xfff
#define B_AX_ACH5_HOST_IDX_SH       0
#define B_AX_ACH5_HOST_IDX_MSK      0xfff

#define R_AX_ACH6_TXBD_IDX          0x1070
#define B_AX_ACH6_HW_IDX_SH         16
#define B_AX_ACH6_HW_IDX_MSK        0xfff
#define B_AX_ACH6_HOST_IDX_SH       0
#define B_AX_ACH6_HOST_IDX_MSK      0xfff

#define R_AX_ACH7_TXBD_IDX          0x1074
#define B_AX_ACH7_HW_IDX_SH         16
#define B_AX_ACH7_HW_IDX_MSK        0xfff
#define B_AX_ACH7_HOST_IDX_SH       0
#define B_AX_ACH7_HOST_IDX_MSK      0xfff

#define R_AX_CH8_TXBD_IDX           0x1078
#define B_AX_CH8_HW_IDX_SH          16
#define B_AX_CH8_HW_IDX_MSK         0xfff
#define B_AX_CH8_HOST_IDX_SH        0
#define B_AX_CH8_HOST_IDX_MSK       0xfff

#define R_AX_CH9_TXBD_IDX           0x107C
#define B_AX_CH9_HW_IDX_SH          16
#define B_AX_CH9_HW_IDX_MSK         0xfff
#define B_AX_CH9_HOST_IDX_SH        0
#define B_AX_CH9_HOST_IDX_MSK       0xfff

#define R_AX_CH12_TXBD_IDX          0x1080
#define B_AX_CH12_HW_IDX_SH         16
#define B_AX_CH12_HW_IDX_MSK        0xfff
#define B_AX_CH12_HOST_IDX_SH       0
#define B_AX_CH12_HOST_IDX_MSK      0xfff

#define R_AX_TXDMA_ADDR_H           0x10F0
#define B_AX_TXDMA_ADDR_H_SH        0
#define B_AX_TXDMA_ADDR_H_MSK       0xffffffffL

#define R_AX_RXDMA_ADDR_H           0x10F4
#define B_AX_RXDMA_ADDR_H_SH        0
#define B_AX_RXDMA_ADDR_H_MSK       0xffffffffL

#define R_AX_RXQ_RXBD_DESA_L        0x1100
#define B_AX_RXQ_RXBD_DESA_L_SH     0
#define B_AX_RXQ_RXBD_DESA_L_MSK    0xffffffffL

#define R_AX_RXQ_RXBD_DESA_H        0x1104
#define B_AX_RXQ_RXBD_DESA_H_SH     0
#define B_AX_RXQ_RXBD_DESA_H_MSK    0xffffffffL

#define R_AX_RPQ_RXBD_DESA_L        0x1108
#define B_AX_RPQ_RXBD_DESA_L_SH     0
#define B_AX_RPQ_RXBD_DESA_L_MSK    0xffffffffL

#define R_AX_RPQ_RXBD_DESA_H        0x110C
#define B_AX_RPQ_RXBD_DESA_H_SH     0
#define B_AX_RPQ_RXBD_DESA_H_MSK    0xffffffffL

#define R_AX_ACH0_TXBD_DESA_L       0x1110
#define B_AX_ACH0_TXBD_DESA_L_SH    0
#define B_AX_ACH0_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH0_TXBD_DESA_H       0x1114
#define B_AX_ACH0_TXBD_DESA_H_SH    0
#define B_AX_ACH0_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH1_TXBD_DESA_L       0x1118
#define B_AX_ACH1_TXBD_DESA_L_SH    0
#define B_AX_ACH1_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH1_TXBD_DESA_H       0x111C
#define B_AX_ACH1_TXBD_DESA_H_SH    0
#define B_AX_ACH1_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH2_TXBD_DESA_L       0x1120
#define B_AX_ACH2_TXBD_DESA_L_SH    0
#define B_AX_ACH2_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH2_TXBD_DESA_H       0x1124
#define B_AX_ACH2_TXBD_DESA_H_SH    0
#define B_AX_ACH2_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH3_TXBD_DESA_L       0x1128
#define B_AX_ACH3_TXBD_DESA_L_SH    0
#define B_AX_ACH3_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH3_TXBD_DESA_H       0x112C
#define B_AX_ACH3_TXBD_DESA_H_SH    0
#define B_AX_ACH3_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH4_TXBD_DESA_L       0x1130
#define B_AX_ACH4_TXBD_DESA_L_SH    0
#define B_AX_ACH4_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH4_TXBD_DESA_H       0x1134
#define B_AX_ACH4_TXBD_DESA_H_SH    0
#define B_AX_ACH4_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH5_TXBD_DESA_L       0x1138
#define B_AX_ACH5_TXBD_DESA_L_SH    0
#define B_AX_ACH5_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH5_TXBD_DESA_H       0x113C
#define B_AX_ACH5_TXBD_DESA_H_SH    0
#define B_AX_ACH5_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH6_TXBD_DESA_L       0x1140
#define B_AX_ACH6_TXBD_DESA_L_SH    0
#define B_AX_ACH6_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH6_TXBD_DESA_H       0x1144
#define B_AX_ACH6_TXBD_DESA_H_SH    0
#define B_AX_ACH6_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH7_TXBD_DESA_L       0x1148
#define B_AX_ACH7_TXBD_DESA_L_SH    0
#define B_AX_ACH7_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_ACH7_TXBD_DESA_H       0x114C
#define B_AX_ACH7_TXBD_DESA_H_SH    0
#define B_AX_ACH7_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_CH8_TXBD_DESA_L        0x1150
#define B_AX_CH8_TXBD_DESA_L_SH     0
#define B_AX_CH8_TXBD_DESA_L_MSK    0xffffffffL

#define R_AX_CH8_TXBD_DESA_H        0x1154
#define B_AX_CH8_TXBD_DESA_H_SH     0
#define B_AX_CH8_TXBD_DESA_H_MSK    0xffffffffL

#define R_AX_CH9_TXBD_DESA_L        0x1158
#define B_AX_CH9_TXBD_DESA_L_SH     0
#define B_AX_CH9_TXBD_DESA_L_MSK    0xffffffffL

#define R_AX_CH9_TXBD_DESA_H        0x115C
#define B_AX_CH9_TXBD_DESA_H_SH     0
#define B_AX_CH9_TXBD_DESA_H_MSK    0xffffffffL

#define R_AX_CH12_TXBD_DESA_L       0x1160
#define B_AX_CH12_TXBD_DESA_L_SH    0
#define B_AX_CH12_TXBD_DESA_L_MSK   0xffffffffL

#define R_AX_CH12_TXBD_DESA_H       0x1164
#define B_AX_CH12_TXBD_DESA_H_SH    0
#define B_AX_CH12_TXBD_DESA_H_MSK   0xffffffffL

#define R_AX_ACH0_BDRAM_CTRL        0x1200
#define B_AX_ACH0_BDRAM_MIN_SH      16
#define B_AX_ACH0_BDRAM_MIN_MSK     0xff
#define B_AX_ACH0_BDRAM_MAX_SH      8
#define B_AX_ACH0_BDRAM_MAX_MSK     0xff
#define B_AX_ACH0_BDRAM_SIDX_SH     0
#define B_AX_ACH0_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH1_BDRAM_CTRL        0x1204
#define B_AX_ACH1_BDRAM_MIN_SH      16
#define B_AX_ACH1_BDRAM_MIN_MSK     0xff
#define B_AX_ACH1_BDRAM_MAX_SH      8
#define B_AX_ACH1_BDRAM_MAX_MSK     0xff
#define B_AX_ACH1_BDRAM_SIDX_SH     0
#define B_AX_ACH1_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH2_BDRAM_CTRL        0x1208
#define B_AX_ACH2_BDRAM_MIN_SH      16
#define B_AX_ACH2_BDRAM_MIN_MSK     0xff
#define B_AX_ACH2_BDRAM_MAX_SH      8
#define B_AX_ACH2_BDRAM_MAX_MSK     0xff
#define B_AX_ACH2_BDRAM_SIDX_SH     0
#define B_AX_ACH2_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH3_BDRAM_CTRL        0x120C
#define B_AX_ACH3_BDRAM_MIN_SH      16
#define B_AX_ACH3_BDRAM_MIN_MSK     0xff
#define B_AX_ACH3_BDRAM_MAX_SH      8
#define B_AX_ACH3_BDRAM_MAX_MSK     0xff
#define B_AX_ACH3_BDRAM_SIDX_SH     0
#define B_AX_ACH3_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH4_BDRAM_CTRL        0x1210
#define B_AX_ACH4_BDRAM_MIN_SH      16
#define B_AX_ACH4_BDRAM_MIN_MSK     0xff
#define B_AX_ACH4_BDRAM_MAX_SH      8
#define B_AX_ACH4_BDRAM_MAX_MSK     0xff
#define B_AX_ACH4_BDRAM_SIDX_SH     0
#define B_AX_ACH4_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH5_BDRAM_CTRL        0x1214
#define B_AX_ACH5_BDRAM_MIN_SH      16
#define B_AX_ACH5_BDRAM_MIN_MSK     0xff
#define B_AX_ACH5_BDRAM_MAX_SH      8
#define B_AX_ACH5_BDRAM_MAX_MSK     0xff
#define B_AX_ACH5_BDRAM_SIDX_SH     0
#define B_AX_ACH5_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH6_BDRAM_CTRL        0x1218
#define B_AX_ACH6_BDRAM_MIN_SH      16
#define B_AX_ACH6_BDRAM_MIN_MSK     0xff
#define B_AX_ACH6_BDRAM_MAX_SH      8
#define B_AX_ACH6_BDRAM_MAX_MSK     0xff
#define B_AX_ACH6_BDRAM_SIDX_SH     0
#define B_AX_ACH6_BDRAM_SIDX_MSK    0xff

#define R_AX_ACH7_BDRAM_CTRL        0x121C
#define B_AX_ACH7_BDRAM_MIN_SH      16
#define B_AX_ACH7_BDRAM_MIN_MSK     0xff
#define B_AX_ACH7_BDRAM_MAX_SH      8
#define B_AX_ACH7_BDRAM_MAX_MSK     0xff
#define B_AX_ACH7_BDRAM_SIDX_SH     0
#define B_AX_ACH7_BDRAM_SIDX_MSK    0xff

#define R_AX_CH8_BDRAM_CTRL         0x1220
#define B_AX_CH8_BDRAM_MIN_SH       16
#define B_AX_CH8_BDRAM_MIN_MSK      0xff
#define B_AX_CH8_BDRAM_MAX_SH       8
#define B_AX_CH8_BDRAM_MAX_MSK      0xff
#define B_AX_CH8_BDRAM_SIDX_SH      0
#define B_AX_CH8_BDRAM_SIDX_MSK     0xff

#define R_AX_CH9_BDRAM_CTRL         0x1224
#define B_AX_CH9_BDRAM_MIN_SH       16
#define B_AX_CH9_BDRAM_MIN_MSK      0xff
#define B_AX_CH9_BDRAM_MAX_SH       8
#define B_AX_CH9_BDRAM_MAX_MSK      0xff
#define B_AX_CH9_BDRAM_SIDX_SH      0
#define B_AX_CH9_BDRAM_SIDX_MSK     0xff

#define R_AX_CH12_BDRAM_CTRL        0x1228
#define B_AX_CH12_BDRAM_MIN_SH      16
#define B_AX_CH12_BDRAM_MIN_MSK     0xff
#define B_AX_CH12_BDRAM_MAX_SH      8
#define B_AX_CH12_BDRAM_MAX_MSK     0xff
#define B_AX_CH12_BDRAM_SIDX_SH     0
#define B_AX_CH12_BDRAM_SIDX_MSK    0xff

/*--------------------Define -------------------------------------------*/
// Config
//#define CONFIG_TX_TRUNC_MODE                1
//#define CONFIG_RX_TRUNC_MODE                1
//#define CONFIG_SEPARATION_MODE              1
//#define CONFIG_DONT_DEFAULT_SETTING         1

#define CACHE_LINE_SIZE                         0x40
#define TXBD_4K_BOUNDARY                        0x1000
#define MAX_WD_PAGE_SIZE                        64  // 64 or 128 bytes

// Config
#define PCIEDMA_TX_CH_NUM                       3  // Test code only try in channel ACH0/ACH1/CH12
#define PCIEDMA_RX_CH_NUM                       2

#if defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RX_APPLEN                       0x100  // To be defined
#endif

// TXBD/RXBD related
// TXBD structure size
#if !defined(CONFIG_TX_TRUNC_MODE)
#define PCIEDMA_TXBD_TYPE_SIZE                  12
#else
#define PCIEDMA_TXBD_TYPE_SIZE                  8
#endif

// RXBD structure size
#if !defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RXBD_TYPE_SIZE                  12
#elif defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RXBD_TYPE_SIZE                  8
#elif !defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RXBD_TYPE_SIZE                  20
#elif defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RXBD_TYPE_SIZE                  12
#endif
#define PCIEDMA_RX_BD_INFO_TYPE_SIZE            4

// Each channel may use different TXBD/RXBD number, to be defined
#define PCIEDMA_CH_TXBD_NUM                     64
#define PCIEDMA_CH_RXBD_NUM                     32

#define PCIEDMA_CH_TXBD_TOTAL_SIZE              ALIGN(PCIEDMA_CH_TXBD_NUM * PCIEDMA_TXBD_TYPE_SIZE, CACHE_LINE_SIZE)
#define PCIEDMA_CH_RXBD_TOTAL_SIZE              ALIGN(PCIEDMA_CH_RXBD_NUM * PCIEDMA_RXBD_TYPE_SIZE, CACHE_LINE_SIZE)

#define PCIEDMA_TX_BDRAM_NUM                    256
#define PCIEDMA_TX_BDRAM_MIN_SIZE               2


// TX buffer size
#define PCIEDMA_TX_BUF_TYPE_WD_BODY_SIZE        24
#define PCIEDMA_TX_BUF_TYPE_WD_INFO_SIZE        24
#define PCIEDMA_TX_BUF_TYPE_WP_INFO_SIZE        16
#define PCIEDMA_TX_BUF_TYPE_PAYLOAD_SIZE        64

#define PCIEDMA_TX_BUF_TOTAL_SIZE               0x1000

// RX buffer size
#if defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RX_BUF_P1_SIZE                  ALIGN(0x180, CACHE_LINE_SIZE) // Must large than PCIEDMA_RX_APPLEN
#define PCIEDMA_RX_BUF_P1_TOTAL_SIZE            (PCIEDMA_CH_RXBD_NUM * PCIEDMA_RX_BUF_P1_SIZE)    // To be defined
#endif
#define PCIEDMA_RX_BUF_P2_SIZE                  ALIGN(0x400, CACHE_LINE_SIZE) // 1k, to be defined
#define PCIEDMA_RX_BUF_P2_TOTAL_SIZE            (PCIEDMA_CH_RXBD_NUM * PCIEDMA_RX_BUF_P2_SIZE)    // To be defined

// Flow control

#define PCIEDMA_TXBD_ACH0_BASEADDR              (u32)(&PCIEDMA_TXBD[0])
#define PCIEDMA_TXBD_ACH1_BASEADDR              (u32)(PCIEDMA_TXBD_ACH0_BASEADDR + PCIEDMA_CH_TXBD_TOTAL_SIZE)
#define PCIEDMA_TXBD_CH12_BASEADDR              (u32)(PCIEDMA_TXBD_ACH1_BASEADDR + PCIEDMA_CH_TXBD_TOTAL_SIZE)
#define PCIEDMA_RXBD_RXQ_BASEADDR               (u32)(&PCIEDMA_RXBD[0])
#define PCIEDMA_RXBD_RPQ_BASEADDR               (u32)(PCIEDMA_RXBD_RXQ_BASEADDR + PCIEDMA_CH_RXBD_TOTAL_SIZE)

#define PCIEDMA_TX_BUF_BASEADDR                 (u32)(&PCIEDMA_TX_BUF[0])
#if defined(CONFIG_SEPARATION_MODE)
#define PCIEDMA_RX_BUF_P1_BASEADDR              (u32)(&PCIEDMA_RX_BUF_P1[0])
#endif
#define PCIEDMA_RXQ_BUF_P2_BASEADDR             (u32)(&PCIEDMA_RX_BUF_P2[0])
#define PCIEDMA_RPQ_BUF_P2_BASEADDR             (u32)(PCIEDMA_RXQ_BUF_P2_BASEADDR + PCIEDMA_RX_BUF_P2_TOTAL_SIZE)


#define MAX_USED_TXBD_NUM                       32

/*--------------------Define Enum---------------------------------------*/
typedef enum PCIE8852A_VERI_PARA_TYPE_ {
	PCIE8852A_VERI_TYPE_INIT        = 0,
    PCIE8852A_VERI_TYPE_TX          = 1,
    PCIE8852A_VERI_TYPE_RX          = 2,
    PCIE8852A_VERI_TYPE_FWDL        = 3,
	PCIE8852A_VERI_TYPE_END         = 4
}PCIE8852A_VERI_PARA_TYPE, *PPCIE8852A_VERI_PARA_TYPE;

enum PCIEDMA_MODE {
    PCIEDMA_MODE_INTERRUPT = 0,
    PCIEDMA_MODE_POLLING =   1
};

enum PCIEDMA_TX_CH {
	PCIEDMA_TX_ACH0 = 0,
    PCIEDMA_TX_ACH1 = 1,
    PCIEDMA_TX_CH12 = 2,
	PCIEDMA_TX_CH_END = PCIEDMA_TX_CH_NUM
};

enum PCIEDMA_RX_CH {
	PCIEDMA_RX_RXQ = 0,
    PCIEDMA_RX_RPQ = 1,
	PCIEDMA_RX_CH_END = PCIEDMA_RX_CH_NUM
};

enum PCIEDMA_TX_BUF_TYPE {
    PCIEDMA_TX_BUF_TYPE_WD_BODY = 0,
    PCIEDMA_TX_BUF_TYPE_WD_OTHER = 1,
    PCIEDMA_TX_BUF_TYPE_PAYLOAD = 2
};

/*--------------------Define MACRO--------------------------------------*/
#define OFFSET_VALUE(_v, _f_msk, _f_sh) (((_v) & _f_msk) << _f_sh)
#define SET_CLR_WORD(_w, _v, _f_msk, _f_sh)                                                   \
    (((_w) & ~((_f_msk) << (_f_sh))) | (((_v) & (_f_msk)) << (_f_sh)))
#define SET_WORD(_v, _f_msk, _f_sh) (((_v) & (_f_msk)) << (_f_sh))
#define GET_FIELD(_w, _f_msk, _f_sh)    (((_w) >> (_f_sh)) & (_f_msk))

#if !defined(CONFIG_TX_TRUNC_MODE)
#define FILL_TXBD(txbd_p, len, ls, addr_l, addr_h)                                          \
    txbd_p->dword0 = SET_WORD(len, AX_TXBD_LENGTH_MSK, AX_TXBD_LENGTH_SH) | (ls ? AX_TXBD_LS : 0);   \
    txbd_p->dword1 = SET_WORD(addr_l, AX_TXBD_PHY_ADDR_L_MSK, AX_TXBD_PHY_ADDR_L_SH);             \
    txbd_p->dword2 = SET_WORD(addr_h, AX_TXBD_PHY_ADDR_H_MSK, AX_TXBD_PHY_ADDR_H_SH);

#define FILL_ADDR_INFO(addr_info_p, len, info_num, ls, msdu_ls, addr_l, addr_h)                         \
    addr_info_p->dword0 =   SET_WORD(len, AX_ADDRINFO_N_LENGTH_MSK, AX_ADDRINFO_N_LENGTH_SH) |          \
                            SET_WORD(info_num, AX_ADDRINFO_N_INFO_NUM_MSK, AX_ADDRINFO_N_INFO_NUM_SH) | \
                            (ls ? AX_ADDRINFO_N_LS : 0) | (msdu_ls ? AX_ADDRINFO_N_MSDU_LS : 0);        \
    addr_info_p->dword1 = SET_WORD(addr_l, AX_ADDRINFO_N_PHY_ADDR_L_MSK, AX_ADDRINFO_N_PHY_ADDR_L_SH);  \
    addr_info_p->dword2 = SET_WORD(addr_h, AX_ADDRINFO_N_PHY_ADDR_H_MSK, AX_ADDRINFO_N_PHY_ADDR_H_SH);
#else
#define FILL_TXBD(txbd_p, len, ls, addr_l)                                          \
    txbd_p->dword0 = SET_WORD(len, AX_TXBD_LENGTH_MSK, AX_TXBD_LENGTH_SH) | (ls ? AX_TXBD_LS : 0);   \
    txbd_p->dword1 = SET_WORD(addr_l, AX_TXBD_PHY_ADDR_L_MSK, AX_TXBD_PHY_ADDR_L_SH);

#define FILL_ADDR_INFO(addr_info_p, len, info_num, ls, msdu_ls, addr_l)                                 \
    addr_info_p->dword0 =   SET_WORD(len, AX_ADDRINFO_T_LENGTH_MSK, AX_ADDRINFO_T_LENGTH_SH) |          \
                            SET_WORD(info_num, AX_ADDRINFO_T_INFO_NUM_MSK, AX_ADDRINFO_T_INFO_NUM_SH) | \
                            (ls ? AX_ADDRINFO_T_LS : 0) | (msdu_ls ? AX_ADDRINFO_T_MSDU_LS : 0);        \
    addr_info_p->dword1 = SET_WORD(addr_l, AX_ADDRINFO_T_PHY_ADDR_L_MSK, AX_ADDRINFO_T_PHY_ADDR_L_SH);
#endif

#if !defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
#define FILL_RXBD(rxbd_p, len, addr_l, addr_h)                                          \
    rxbd_p->dword0 = SET_WORD(len, AX_RXBD_PKT_RXBUFFSIZE_MSK, AX_RXBD_PKT_RXBUFFSIZE_SH);    \
    rxbd_p->dword1 = SET_WORD(addr_l, AX_RXBD_PKT_PHY_ADDR_L_MSK, AX_RXBD_PKT_PHY_ADDR_L_SH); \
    rxbd_p->dword2 = SET_WORD(addr_h, AX_RXBD_PKT_PHY_ADDR_H_MSK, AX_RXBD_PKT_PHY_ADDR_H_SH);

#define GET_RXBD_SIZE(rxbd_p)       GET_FIELD(rxbd_p->dword0, AX_RXBD_PKT_RXBUFFSIZE_MSK, AX_RXBD_PKT_RXBUFFSIZE_SH)
#define GET_RXBD_ADDR_L(rxbd_p)     GET_FIELD(rxbd_p->dword1, AX_RXBD_PKT_PHY_ADDR_L_MSK, AX_RXBD_PKT_PHY_ADDR_L_SH)

#elif defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
#define FILL_RXBD(rxbd_p, len, addr_l)                                                  \
    rxbd_p->dword0 = SET_WORD(len, AX_RXBD_PKT_RXBUFFSIZE_MSK, AX_RXBD_PKT_RXBUFFSIZE_SH);    \
    rxbd_p->dword1 = SET_WORD(addr_l, AX_RXBD_PKT_PHY_ADDR_L_MSK, AX_RXBD_PKT_PHY_ADDR_L_SH);

#define GET_RXBD_SIZE(rxbd_p)       GET_FIELD(rxbd_p->dword0, AX_RXBD_PKT_RXBUFFSIZE_MSK, AX_RXBD_PKT_RXBUFFSIZE_SH)
#define GET_RXBD_ADDR_L(rxbd_p)     GET_FIELD(rxbd_p->dword1, AX_RXBD_PKT_PHY_ADDR_L_MSK, AX_RXBD_PKT_PHY_ADDR_L_SH)

#elif !defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
#define FILL_RXBD(rxbd_p, len1, len2, addr1_l, addr1_h, addr2_l, addr2_h)                           \
    rxbd_p->dword0 = SET_WORD(len1, AX_RXBD_SEPN_RXP1BUFFSIZE_MSK, AX_RXBD_SEPN_RXP1BUFFSIZE_SH) |        \
                     SET_WORD(len2, AX_RXBD_SEPN_RXP2BUFFSIZE_MSK, AX_RXBD_SEPN_RXP2BUFFSIZE_SH);         \
    rxbd_p->dword1 = SET_WORD(addr1_l, AX_RXBD_SEPN_PHY_ADDR_P1_L_MSK, AX_RXBD_SEPN_PHY_ADDR_P1_L_SH);    \
    rxbd_p->dword2 = SET_WORD(addr1_h, AX_RXBD_SEPN_PHY_ADDR_P1_H_MSK, AX_RXBD_SEPN_PHY_ADDR_P1_H_SH);    \
    rxbd_p->dword3 = SET_WORD(addr2_l, AX_RXBD_SEPN_PHY_ADDR_P2_L_MSK, AX_RXBD_SEPN_PHY_ADDR_P2_L_SH);    \
    rxbd_p->dword4 = SET_WORD(addr2_h, AX_RXBD_SEPN_PHY_ADDR_P2_H_MSK, AX_RXBD_SEPN_PHY_ADDR_P2_H_SH);

#define GET_RXBD_SIZE1(rxbd_p)      GET_FIELD(rxbd_p->dword0, AX_RXBD_SEPN_RXP1BUFFSIZE_MSK, AX_RXBD_SEPN_RXP1BUFFSIZE_SH)
#define GET_RXBD_SIZE2(rxbd_p)      GET_FIELD(rxbd_p->dword0, AX_RXBD_SEPN_RXP2BUFFSIZE_MSK, AX_RXBD_SEPN_RXP2BUFFSIZE_SH)
#define GET_RXBD_ADDR1_L(rxbd_p)    GET_FIELD(rxbd_p->dword1, AX_RXBD_SEPN_PHY_ADDR_P1_L_MSK, AX_RXBD_SEPN_PHY_ADDR_P1_L_SH)
#define GET_RXBD_ADDR2_L(rxbd_p)    GET_FIELD(rxbd_p->dword3, AX_RXBD_SEPN_PHY_ADDR_P2_L_MSK, AX_RXBD_SEPN_PHY_ADDR_P2_L_SH)

#elif defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
#define FILL_RXBD(rxbd_p, len1, len2, addr1_l, addr2_l)                                             \
    rxbd_p->dword0 = SET_WORD(len1, AX_RXBD_SEPT_RXP1BUFFSIZE_MSK, AX_RXBD_SEPT_RXP1BUFFSIZE_SH) |        \
                     SET_WORD(len2, AX_RXBD_SEPT_RXP2BUFFSIZE_MSK, AX_RXBD_SEPT_RXP2BUFFSIZE_SH);         \
    rxbd_p->dword1 = SET_WORD(addr1_l, AX_RXBD_SEPT_PHY_ADDR_P1_L_MSK, AX_RXBD_SEPT_PHY_ADDR_P1_L_SH);    \
    rxbd_p->dword2 = SET_WORD(addr2_l, AX_RXBD_SEPT_PHY_ADDR_P2_L_MSK, AX_RXBD_SEPT_PHY_ADDR_P2_L_SH);

#define GET_RXBD_SIZE1(rxbd_p)      GET_FIELD(rxbd_p->dword0, AX_RXBD_SEPT_RXP1BUFFSIZE_MSK, AX_RXBD_SEPT_RXP1BUFFSIZE_SH)
#define GET_RXBD_SIZE2(rxbd_p)      GET_FIELD(rxbd_p->dword0, AX_RXBD_SEPT_RXP2BUFFSIZE_MSK, AX_RXBD_SEPT_RXP2BUFFSIZE_SH)
#define GET_RXBD_ADDR1_L(rxbd_p)    GET_FIELD(rxbd_p->dword1, AX_RXBD_SEPT_PHY_ADDR_P1_L_MSK, AX_RXBD_SEPT_PHY_ADDR_P1_L_SH)
#define GET_RXBD_ADDR2_L(rxbd_p)    GET_FIELD(rxbd_p->dword2, AX_RXBD_SEPT_PHY_ADDR_P2_L_MSK, AX_RXBD_SEPT_PHY_ADDR_P2_L_SH)

#endif

#define GET_RX_BD_INFO_TAG(rx_bd_info_p)    GET_FIELD(rx_bd_info_p->dword0, AX_BD_INFO_TAG_MSK, AX_BD_INFO_TAG_SH)
#define GET_RX_BD_INFO_SIZE(rx_bd_info_p)   GET_FIELD(rx_bd_info_p->dword0, AX_BD_INFO_HW_WRITE_SIZE_MSK, AX_BD_INFO_HW_WRITE_SIZE_SH)

// common
#define ALIGN(x, align_size)	(((x) + (align_size) - 1) & ~((align_size) - 1))

// Memory mapping
#define vir2phy(_addr)                  ((_addr) & 0xFFFFFFFF)
#define phy2vir(_addr)                  ((_addr) | 0x00000000)

/*--------------------Define Struct-------------------------------------*/
typedef struct _TXBD_TYPE_
{
    u32 dword0;
    u32 dword1;
#if !defined(CONFIG_TX_TRUNC_MODE)
    u32 dword2;
#endif
} TXBD_TYPE, *PTXBD_TYPE;

typedef struct _ADDR_INFO_TYPE_
{
    u32 dword0;
    u32 dword1;
#if !defined(CONFIG_TX_TRUNC_MODE)
    u32 dword2;
#endif
} ADDR_INFO_TYPE, *PADDR_INFO_TYPE;


typedef struct _RXBD_TYPE_
{
#if !defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
    u32 dword0;
    u32 dword1;
    u32 dword2;
#elif defined(CONFIG_RX_TRUNC_MODE) && !defined(CONFIG_SEPARATION_MODE)
    u32 dword0;
    u32 dword1;
#elif !defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
    u32 dword0;
    u32 dword1;
    u32 dword2;
    u32 dword3;
    u32 dword4;
#elif defined(CONFIG_RX_TRUNC_MODE) && defined(CONFIG_SEPARATION_MODE)
    u32 dword0;
    u32 dword1;
    u32 dword2;
#endif
} RXBD_TYPE, *PRXBD_TYPE;

typedef struct _RX_BD_INFO_TYPE_
{
    u32 dword0;
} RX_BD_INFO_TYPE, *PRX_BD_INFO_TYPE;

typedef struct _txdesc_t_ {  //64-byte mode
	u32 dword0; //DW0~DW5 : WD BODY
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6; //DW6~DW11 : WD INFO
	u32 dword7;
	u32 dword8;
	u32 dword9;
	u32 dword10;
	u32 dword11;
	u32 dword12; // DW12~DW13: PCIE SEQ NUM
	u32 dword13;
	u32 dword14; // DW14~DW15: ADDR INFO
	u32 dword15;
}txdesc_t;

typedef struct _PCIEDMA_TX_PARA_ {
    u8* buf_addr[MAX_USED_TXBD_NUM];
    u16 buf_len[MAX_USED_TXBD_NUM];
    u8 ls[MAX_USED_TXBD_NUM];
    u8 channel;
    u8 used_txbd_num;
    u8 mode;
} PCIEDMA_TX_PARA;


/*--------------------Export global variable----------------------------*/

/*--------------------Function declaration------------------------------*/



#endif  //_RTk_PCIE_8852A_
