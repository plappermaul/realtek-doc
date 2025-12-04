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
#ifndef __CA_NI_H__
#define __CA_NI_H__

#if 0//yocto
#include <linux/platform_device.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#else//sd1 uboot - TBD
#endif

#include "aal_ni.h"

#include "ca_ni_common.h"
#include "ca_ethtool.h"
#include "ca_ni_tx.h"

#define DRV_NAME	"Cortina Network Interface Driver"
#define DRV_VERSION	"0.0.1"

#define CA_ENABLE			1
#define CA_DISABLE			0

/* max number of interfaces supportted is 8 the real number of interfaces should be specified in startup config */
#define CA_NI_MAX_INTF			8

/* the min DMA size of G3 DMA LSO is 34 */
#define CA_NI_MIN_DMA_SIZE		34

#define CA_NI_TX_TIMEOUT		(6 * HZ)
#define CA_NI_XRAM_RX_INSTANCE		8

#define CA_NI_NUM_OF_CPU		8

#define REG32(addr)			(*(volatile u32*)(addr))

#define CA_NI_XRAM_BASE                                    0xF4500000

/* defined in "ca_ni_tx.h" */
struct ca_ni_priv;
struct dma_swtxq_t;

/* the port configure should be handle in Cortina port APIs directly */
typedef struct ca_port_cfg_info {
	u8 link;
	u8 autoneg;
	u16 speed;
	u8 duplex;
	u8 flowctrl;
	//u8 phy_mode;
	u8 num_phy_port;
	u8 port_id[CA_NI_NUM_OF_PORT];
	u8 rx_ldpid;			/* used to tell where the incoming packet come from, 0x10=from LAN port, 0x11=from WAN port */
	u8 tx_ldpid;			/* used to tell where the ougoing packet will go to, 0x18=go to LAN port, 0x19=go to WAN port */
	//u8 phy_addr;
	int irq;
	u8 *mac_addr;
	u8 rmii_clk_src;
} ca_port_cfg_info_t;

#if 0//yocto
typedef struct ca_eth_private {
	struct net_device *dev;
	struct platform_device *pdev;
	struct mii_bus *mdio_bus;
	struct phy_device *phydev;
	struct napi_struct napi;

	void __iomem *base;		/* Ethernet controller base address */

	u8 status;			/* enable/disable ??? */
	int ni_driver_state;		/* 1: ready, 0: initializing */
	u32 mdio_inited;		/* 1: mdio is inited, 0: mdio is not inited */

	/*
	 * Device status & features
	 */
	ca_port_cfg_info_t port_cfg;

	u8 cpu_port;			/* indicate which cpu port attached to this interface */

	u32 an_bng_mac_idx;
	u32 an_bng_mac_idx_pe0;
	u32 an_bng_mac_idx_pe1;
	u32 msg_enable;

	/*
	 * Statistics
	 */
	struct ca_ethtool_stats ni_stats;
	struct net_device_stats ifStatics;
	u8 rx_checksum;

	/*
	 * Rx-only resource
	 */


	/*
	 * Tx-only resource
	 */
	unsigned long tx_completion_queues;
	struct timer_list tx_completion_timer;
	struct ca_ni_priv mgmt_priv;		/* private data for management mode */

	/*
	 * Hardware-specific parameters
	 */
	bool  use_default_lso_para0;
	u32 default_lso_para0;
	u32 deep_q:1;
	u32 dma_l3fe:1;

	/*
	 * lock for access to global data
	 */
	///      these locks are used for device based not NI driver based
	spinlock_t stats_lock;
	spinlock_t lock;

#ifdef CONFIG_CA_NE_VIRTUAL_NI
	/*
	 * virtual NI
	 */
	u16 vni_vlan_proto;		/// ca_virt_ni_t.vlan_proto
	u16 vni_vlan_tci;		/// ca_virt_ni_t.vlan_tci
#endif

} ca_eth_private_t;

typedef struct ni_private {
	struct net_device *dev[CA_NI_MAX_INTF];
	dma_swtxq_t swtxq[CA_NI_NUM_OF_CPU][CA_NI_DMA_LSO_TXQ_NUM];

	/* default values for NI registers */
	u32 intr_cpuxram_rxpkt_mask;
	u32 intr_cpuxram_txpkt_mask;
	u32 intr_sch_mask;

        /*
         * L3QM buffer management
         */
        u8 *l3qm_eb_base_addr;          		/* virtual address of EB pool for NI ports */
        dma_addr_t l3qm_eb_base_dma_addr;          	/* dma address of EB pool for NI ports */
        u32 *l3qm_cpu_epp_base_addr[CA_NI_TOTAL_CPU_PORT * CA_NI_NUM_VOQ_PER_PORT];  		/* virtual address of CPU EPP for transmiting */
        dma_addr_t l3qm_cpu_epp_base_dma_addr[CA_NI_TOTAL_CPU_PORT * CA_NI_NUM_VOQ_PER_PORT];  /* dma address of CPU EPP for transmiting */

	/*
	 * task for reset TX queues
	 */
	struct work_struct reset_task;

	/* refill threshold irq number */
	u16 	refill_req_0;
	u16 	refill_req_1;

	/* used for dma_alloc_coherent() */
	struct platform_device *pdev;
} ni_info_t;

void ca_ni_refill_eq_buf_pool(struct net_device *dev, u8 cpu_port, u8 eqid, int cnt);
ca_status_t ca_ni_intf_get_ports(struct net_device *dev, ca_uint8_t *num_ports, int *ports, ca_uint8_t *is_wan_intf);
struct net_device * ca_ni_virtual_instance_alloc(int cpu_idx, char *name);
ca_status_t ca_ni_virtual_instance_free(int cpu_idx);
ca_status_t ca_ni_virtual_instance_open(int cpu_idx);
ca_status_t ca_ni_virtual_instance_close(int cpu_idx);
ca_status_t ca_ni_virtual_instance_xmit(int cpu_idx, struct sk_buff *skb, ca_ni_tx_config_t *tx_config);

extern ni_info_t ni_info_data;
#else//sd1 uboot for 98f
#endif
extern int ni_use_fbm;
extern int ca_ni_mgmt_mode;
extern u8 ca_ni_eth_mac[CA_NI_MAX_INTF][6];
extern int ca_ni_num_intf;
extern u32 ca_ni_debug;

/* spin lock for global resources */
extern spinlock_t  ca_ni_rx_refill_lock;

#endif /* __CA_NI_H__ */

