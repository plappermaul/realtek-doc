/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * aal_ni.h: Hardware Abstraction Layer for NI driver to access hardware regsiters
 *
 */
#ifndef __AAL_NI_H__
#define __AAL_NI_H__

#include "ca_types.h"
#if 0//Yocto
#include <soc/cortina/registers.h>
#else//98F uboot
#include "g3lite_registers.h"
#endif

#include "aal_common.h"
#include "aal_port.h"

//#define NI_USE_CPU_XRAM                1

#define CA_NI_SRAM_BASE_ADDR            0xC0000000
#define CA_NI_SRAM_SIZE                 SZ_256K
#define CA_NI_DDR_COHERENT_BASE_ADDR    0x400000000

#ifdef CONFIG_ARCH_CORTINA_G3LITE
#define CA_NI_DDR_COHERENT_SIZE         0x1700000		   /* please refer to ca76xx-soc.dtsi */
#else
#define CA_NI_DDR_COHERENT_SIZE         0x1200000		   /* please refer to ca7774-soc.dtsi */
#endif

#define CA_NI_DDR_NONCACHE_BASE_ADDR    0x180000000
#define CA_NI_DDR_NONCACHE_SIZE        	0x2270000                  /* please refer to ca7774-soc.dtsi */

/* port number definition */
#define CA_NI_GE0_PORT      AAL_LPORT_ETH_NI0
#define CA_NI_GE1_PORT      AAL_LPORT_ETH_NI1
#define CA_NI_GE2_PORT      AAL_LPORT_ETH_NI2
#define CA_NI_GE3_PORT      AAL_LPORT_ETH_NI3
#define CA_NI_GE7_PORT      AAL_LPORT_ETH_NI7

#define CA_NI_PORT0     AAL_LPORT_ETH_NI0
#define CA_NI_PORT1     AAL_LPORT_ETH_NI1
#define CA_NI_PORT2     AAL_LPORT_ETH_NI2
#define CA_NI_PORT3     AAL_LPORT_ETH_NI3
#define CA_NI_PORT4     AAL_LPORT_ETH_NI4
#define CA_NI_PORT5     AAL_LPORT_ETH_NI5
#define CA_NI_PORT6     AAL_LPORT_ETH_NI6
#define CA_NI_PORT7     AAL_LPORT_ETH_NI7

#define CA_NI_WAN_PORT 	CA_NI_PORT7

#define CA_NI_PPORT_CPU     AAL_PPORT_CPU

#define CA_NI_MGMT_PORT	    CA_NI_GE1_PORT

/* Logical port id definition */
#define CA_NI_CPU_PORT0_LPID    AAL_LPORT_CPU_0	/* lspid=16 */
#define CA_NI_CPU_PORT1_LPID    AAL_LPORT_CPU_1	/* lspid=17 */
#define CA_NI_L2RP_LAN_LPID AAL_LPORT_L2RP_LAN	/* lspid=8 */
#define CA_NI_L2RP_WAN_LPID AAL_LPORT_L2RP_WAN	/* lspid=9 */

#define CA_NI_LAN_L2RP0_LPID    AAL_LPORT_CPU_0	/* Logical Port ID for packet incoming from LAN port */
#define CA_NI_WAN_L2RP0_LPID    AAL_LPORT_CPU_1	/* Logical Port ID for packet incoming from WAN port */
#define CA_NI_L2_L3_LAN_LPID    0x19	/* Logical Port ID for packet outgoing to LAN port */
#define CA_NI_L2_L3_WAN_LPID    0x18	/* Logical Port ID for packet outgoing to WAN port */

#define CA_NI_WAN_LPID		CA_NI_WAN_PORT		/* ldpid=7 */
#define CA_NI_LAN_LPID		AAL_LPORT_ETH_NI6	/* ldpid=6 */

/* number of NI port */
#define CA_NI_NUM_OF_PORT       (CONFIG_CA_NE_AAL_PORT_NUM_ETH)

#define CPU_PORT0_VOQ_BASE              0
#define CPU_PORT1_VOQ_BASE              8
#define CPU_PORT2_VOQ_BASE              16
#define CPU_PORT3_VOQ_BASE              24
#define CPU_PORT4_VOQ_BASE              32
#define CPU_PORT5_VOQ_BASE              40
#define CPU_PORT6_VOQ_BASE              48
#define CPU_PORT7_VOQ_BASE              56

#define CPU_PORT0_IRQ               1
#define CPU_PORT1_IRQ               2
#define CPU_PORT2_IRQ               3
#define CPU_PORT3_IRQ               4
#define CPU_PORT4_IRQ               5
#define CPU_PORT5_IRQ               6
#define CPU_PORT6_IRQ               7
#define CPU_PORT7_IRQ               8

#define CPU_PORT0_INST                   0
#define CPU_PORT1_INST                   1
#define CPU_PORT2_INST                   2
#define CPU_PORT3_INST                   3
#define CPU_PORT4_INST                   4
#define CPU_PORT5_INST                   5
#define CPU_PORT6_INST                   6
#define CPU_PORT7_INST                   7

#define CA_NI_SKB_PKT_LEN   2048	/* 2 * 1024, buffer size of Empty Buffer */
#define CA_NI_EB_BUF_SIZE          CA_NI_SKB_PKT_LEN	/* Empty Buffer size */
#define CA_NI_TOTAL_EB_POOL        16
#define CA_NI_TOTAL_EB_BUF_NUM     1024
#define CA_NI_TOTAL_EB_PROFILE      8

#define CA_NI_TOTAL_CPU_PORT        8	/* 8 CPU ports */
#define CA_NI_TOTAL_NI_PORT         8	/* 8 NI ports */
#define CA_NI_NUM_VOQ_PER_PORT      8	/* 8 VoQs per port */

#define CA_NI_PE0_PORT		(CA_NI_TOTAL_CPU_PORT-2)
#define CA_NI_PE1_PORT		(CA_NI_TOTAL_CPU_PORT-1)

#define CA_NI_CPU_EPP_CMD_SIZE      4
#define CA_NI_CPU_EPP_FIFO_ENTRIES  1*1024	/* 1K FIFO entries */

#define CA_NI_INIT_DONE_LOOP    1000000

#define CA_NI_MAX_FRAME_SIZE    9022	/* 64 ~ 9022 */
#define CA_NI_MIN_FRAME_SIZE    64	/* 64 ~ 9022 */

#define CA_NI_DMA_LSO_TXDESC_NUM	1024
#define CA_NI_DMA_LSO_TXQ_DEPTH 	10
#define CA_NI_DMA_LSO_BASE_DEPTH_LOOP  1000000

/* Defines the base and top address in CPU XRAM for packets from cpu instance 0.*/
#define CA_NI_TX_TOP_ADDR                                    (CA_NI_RX_TOP_ADDR+1+0x00FF)  /* 0x100 * 8-byte = 2K-byte */
#define CA_NI_TX_BASE_ADDR                                   (CA_NI_RX_TOP_ADDR+1)

/* Defines the base and top address in CPU XRAM for packets to cpu instance 0 */
#define CA_NI_RX_TOP_ADDR                                     0x02FF 			     /* 0x300 * 8-byte = 8K-byte */
#define CA_NI_RX_BASE_ADDR                                    0x0000
#define CA_NI_RX_MAX_ADDR_LOC                                 0x2FFF

#define CA_DMA_LSO_REG_READ(addr)		CA_REG_READ((uintptr_t)AAL_DMA_REG_ADDR(addr - DMA_SEC_DMA_GLB_DMA_LSO_CTRL))
#define CA_DMA_LSO_REG_WRITE(data, addr)	CA_REG_WRITE(data, (uintptr_t)AAL_DMA_REG_ADDR(addr - DMA_SEC_DMA_GLB_DMA_LSO_CTRL))

#define CA_GLB_REG_READ(addr)			CA_REG_READ((uintptr_t)AAL_GLB_REG_ADDR(addr - GLOBAL_JTAG_ID))
#define CA_GLB_REG_WRITE(data, addr)		CA_REG_WRITE(data, (uintptr_t)AAL_GLB_REG_ADDR(addr - GLOBAL_JTAG_ID))

#define CA_PER_PHY_REG_READ(addr)		CA_REG_READ((uintptr_t)AAL_PER_PHY_REG_ADDR(addr - PER_MDIO_CFG))
#define CA_PER_PHY_REG_WRITE(data, addr)	CA_REG_WRITE(data, (uintptr_t)AAL_PER_PHY_REG_ADDR(addr - PER_MDIO_CFG))

#if 0
typedef enum {
	CPU_XRAM_CFG_PROFILE_SIMPLE_DEFAULT = 0,
	CPU_XRAM_CFG_PROFILE_SEPARATE_LSPID = 1,
	CPU_XRAM_CFG_PROFILE_PNI_WFO = 2,
	CPU_XRAM_CFG_PROFILE_MAX
} cpu_xram_config_profile_e;

typedef enum {
	XRAM_DIRECTION_RX = 0,
	XRAM_DIRECTION_TX,
} xram_direction_t;

typedef enum {
	XRAM_INST_0 = 0,
	XRAM_INST_1,
	XRAM_INST_2,		/* Instance 2 - 9 is valid only for RX direction */
	XRAM_INST_3,
	XRAM_INST_4,
	XRAM_INST_5,
	XRAM_INST_6,
	XRAM_INST_7,
	XRAM_INST_8,
	XRAM_INST_MAX,
} xram_inst_t;

typedef enum {
	NI_VOQ_DID_GE0 = 0x0,
	NI_VOQ_DID_GE1 = 0x1,
	NI_VOQ_DID_GE2 = 0x2,
	NI_VOQ_DID_GE3 = 0x3,
	NI_VOQ_DID_WAN = 0x5,
	NI_VOQ_DID_MC = 0x6,
	NI_VOQ_DID_CPU0 = 0x8,
	NI_VOQ_DID_CPU1 = 0x9,
	NI_VOQ_DID_CPU2 = 0xA,
	NI_VOQ_DID_CPU3 = 0xB,
	NI_VOQ_DID_CPU4 = 0xC,
	NI_VOQ_DID_CPU5 = 0xD,
	NI_VOQ_DID_CPU6 = 0xE,
	NI_VOQ_DID_CPU7 = 0xF,
} ni_voq_did_t;			/* The destination selection for the VoQ */
#endif

typedef enum {
        NIRX_L3FE_DEMUX_ID_L3QM = 0,
        NIRX_L3FE_DEMUX_ID_WAN = 1,
        NIRX_L3FE_DEMUX_ID_L2FE = 2,
} nirx_l3fe_demux_id_t;

typedef volatile union {
  struct {
    ca_uint32_t ldpid_0               :  2 ;
    ca_uint32_t ldpid_1               :  2 ;
    ca_uint32_t ldpid_2               :  2 ;
    ca_uint32_t ldpid_3               :  2 ;
    ca_uint32_t ldpid_4               :  2 ;
    ca_uint32_t ldpid_5               :  2 ;
    ca_uint32_t ldpid_6               :  2 ;
    ca_uint32_t ldpid_7               :  2 ;
    ca_uint32_t ldpid_8               :  2 ;
    ca_uint32_t ldpid_9               :  2 ;
    ca_uint32_t ldpid_10              :  2 ;
    ca_uint32_t ldpid_11              :  2 ;
    ca_uint32_t ldpid_12              :  2 ;
    ca_uint32_t ldpid_13              :  2 ;
    ca_uint32_t ldpid_14              :  2 ;
    ca_uint32_t ldpid_15              :  2 ;
  } bf ;
  ca_uint32_t     wrd ;
} ca_ni_rx_l3fe_demux_t;

typedef volatile union {
  struct {
    ca_uint32_t user               :  6 ;
    ca_uint32_t domain             :  2 ;
    ca_uint32_t bar                :  2 ;
    ca_uint32_t snoop              :  4 ;
    ca_uint32_t cache              :  4 ;
    ca_uint32_t qos                :  4 ;
    ca_uint32_t prot               :  3 ;
    ca_uint32_t ace_cmd            :  1 ;
    ca_uint32_t reserved           :  6 ;
  } bf ;
  ca_uint32_t     wrd ;
} ca_ni_dma_lso_read_user_t;

typedef struct {
    ca_uint8_t  dma_to_l3fe;
    ca_uint8_t  wan_rxsel;
    ca_uint8_t l3qmrx_demux_sel;
} aal_ni_hv_glb_internal_port_id_cfg_t;

typedef volatile union {
  struct {
    ca_uint32_t   dma_to_l3fe      :  1 ;
    ca_uint32_t   wan_rxsel        :  1 ;
    ca_uint32_t   l3qmrx_demux_sel :  1 ;
  } bf ;
  ca_uint32_t     wrd ;
} aal_ni_hv_glb_internal_port_id_cfg_mask_t;

/* the connection type between MAC and PHY */
#define CA_NI_MAC_PHY_MII          1
#define CA_NI_MAC_PHY_RGMII_1000   2
#define CA_NI_MAC_PHY_RGMII_100    3
#define CA_NI_MAC_PHY_RMII         5
#define CA_NI_MAC_PHY_SSSMII       7

//#define CA_NI_NORMAL_MODE             0       /* Ethernet interface plays as Normal MAC mode and connects to PHY */
//#define CA_NI_PHY_MODE                1   /* Ethernet interface plays as PHY mode, and connects directly to another MAC */

//#define RESET_NI              0x7f

/* define to be the same as mii.h */
#define CA_NI_TX_FLOW_CTRL         1
#define CA_NI_RX_FLOW_CTRL         2

//#define HW_WR_PTR             0x3FFF
//#define SW_RD_PTR             0x3FFF

#define CA_NI_MAX_FRAME_SIZE        9022
#define CA_NI_MIN_FRAME_SIZE        64

#define CA_NI_MAX_PORT_CALENDAR     96

//#define NI_XRAM_BASE          0xf0700000

//#define XRAM_TX_INSTANCE      2 /* not include DMA LSO */
//#define XRAM_RX_INSTANCE      8 /* not include PTP */

#define CA_NI_GE0_PORT_VOQ_BASE               64
#define CA_NI_GE1_PORT_VOQ_BASE               72
#define CA_NI_GE2_PORT_VOQ_BASE               80
#define CA_NI_GE3_PORT_VOQ_BASE               88
#define CA_NI_PORT4_VOQ_BASE               96
#define CA_NI_PORT5_VOQ_BASE               104
#define CA_NI_PORT6_VOQ_BASE               112
#define CA_NI_PORT7_VOQ_BASE               120

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
#define CA_NI_DMA_LSO_CPU_VP_NUM         2
#else
#define CA_NI_DMA_LSO_CPU_VP_NUM         6	/* 6 DMA LSO VP for CPUs, 2 for Taroko */
#endif

#if 0
#define WAN_PORT_VOQ_BASE       NI_PORT6_VOQ_BASE
#define MC_PORT_VOQ_BASE        NI_PORT7_VOQ_BASE

#define CPU_PORT0_VOQ_BASE              0
#define CPU_PORT1_VOQ_BASE              8
#define CPU_PORT2_VOQ_BASE              16
#define CPU_PORT3_VOQ_BASE              24
#define CPU_PORT4_VOQ_BASE              32
#define CPU_PORT5_VOQ_BASE              40
#define CPU_PORT6_VOQ_BASE              48
#define CPU_PORT7_VOQ_BASE              56
#endif

#define CA_NI_DMA_LSO_RXQ_NUM            8	/* Maximum is 8 */
#define CA_NI_DMA_LSO_TXQ_NUM            8	/* Maximum is 8 */


#if defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define CA_NI_DMA_LSO_VP_NUM             2	/* Maximum is 2 */
#else
#define CA_NI_DMA_LSO_VP_NUM             12	/* Maximum is 12 */
#endif

/* prototype */
ca_status_t aal_ni_set_eth_cfg( /*AUTO-CLI gen ignore */
                                      ca_uint8_t port_id, ca_uint8_t int_cfg,
                                      ca_uint8_t rmii_clk_src);
ca_status_t aal_ni_set_mac_speed_duplex(        /*AUTO-CLI gen ignore */
                                               ca_uint8_t port_id,
                                               ca_uint16_t speed,
                                               ca_uint8_t duplex,
                                               ca_uint16_t mac_interface);
void aal_ni_set_flow_control(   /*AUTO-CLI gen ignore */
                                    ca_uint8_t port_id, ca_uint8_t rx_tx,
                                    ca_uint8_t disable);
ca_status_t aal_ni_set_mac_address(     /*AUTO-CLI gen ignore */
                                          ca_uint8_t port_id,
                                          ca_uint8_t * mac_addr,
                                          int mgmt_mode);
void aal_ni_enable_rx_interrupt(        /*AUTO-CLI gen ignore */
                                       ca_uint16_t cpu_port, ca_uint8_t enable, int mgmt_mode);
void aal_ni_enable_rx_interrupt_by_cpu_port(ca_uint16_t cpu_port, ca_uint8_t enable, int mgmt_mode);
void aal_ni_write_rx_interrupt_status(  /*AUTO-CLI gen ignore */
                                             ca_uint16_t irq_no,
                                             ca_uint32_t status, int mgmt_mode);
ca_uint32_t aal_ni_read_rx_interrupt_status(    /*AUTO-CLI gen ignore */
                                                   ca_uint16_t irq_no, int mgmt_mode);
int aal_ni_has_work(            /*AUTO-CLI gen ignore */
                          int instance, int mgmt_mode);
void aal_ni_init_ni(            /*AUTO-CLI gen ignore */
                           int mgmt_mode, int port_id);
void aal_ni_init_interrupt_cfg( /*AUTO-CLI gen ignore */
                             int mgmt_mode);
void aal_ni_set_flowctrl(       /*AUTO-CLI gen ignore */
                                ca_uint8_t port_id, ca_uint8_t flowctrl);
void aal_ni_set_dma_lso_base_depth_addr(        /*AUTO-CLI gen ignore */
                                               ca_uint8_t instance,
                                               ca_uint8_t txq_id,
                                               ca_uint32_t tx_desc_paddr);

void aal_ni_dump_registers(void);
void aal_ni_set_tx_write_ptr(ca_uint32_t reg, ca_uint32_t value);
ca_uint32_t aal_ni_get_tx_read_ptr(ca_uint32_t reg);
void aal_ni_set_dma_lso_headerA(ca_uint16_t port_no, ca_uint16_t ldpid, ca_uint8_t febypass);
void aal_ni_enable_cpu_epp_interrupt(ca_uint16_t cpu_epp_fifo, ca_uint8_t enable);

ca_status_t aal_ni_hv_glb_internal_port_id_cfg_get(
    CA_IN ca_device_id_t          device_id,
    CA_OUT aal_ni_hv_glb_internal_port_id_cfg_t *cfg
);
ca_status_t aal_ni_hv_glb_internal_port_id_cfg_set(
    CA_IN ca_device_id_t          device_id,
    CA_IN aal_ni_hv_glb_internal_port_id_cfg_mask_t mask,
    CA_IN aal_ni_hv_glb_internal_port_id_cfg_t *cfg
);

extern ca_uint8_t ca_ni_a53_cpu_port_count;
#endif
