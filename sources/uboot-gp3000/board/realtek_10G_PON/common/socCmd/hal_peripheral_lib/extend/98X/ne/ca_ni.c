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
 * ca_ni.c: Cortina Access Network Interface driver
 *
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ":%s:%d: " fmt, __func__, __LINE__

#if 0//yocto
#include <generated/ca_ne_autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
//#include <asm/ca_ni_hook_api.h>
#include <linux/slab.h>

#include <linux/io.h>
#include <linux/reset.h>

#if CONFIG_OF /* DT support */
#include <linux/of.h>
#include <linux/of_device.h>

#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

#include <linux/phy/phy.h>

#include <dt-bindings/soc/cortina_irqs.h>

#endif /* DT support */
#else//sd1 uboot
#include "ca_ne_autoconf.h"
#endif

#include "ca_aal.h"
#include "ca_aal_proc.h"
#include "aal_common.h"
#include "aal_port.h"
#include "aal_ni_l2.h"
#include "aal_ni.h"
#include "aal_l3qm.h"
#include "aal_l3fe.h"

#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU
#include "aal_lpm.h"
#endif

#include "aal_fbm.h"

#include "eth_port.h"
#include "port.h"
#include "sys.h"
#include "scfg.h"

#include "ca_ni.h"
#include "ca_phy.h"
#include "ca_virt_ni.h"
#include "ca_ni_rx.h"
#include "ca_ni_tx.h"
#include "ca_ethtool.h"
#include "ca_ni_proc.h"

#include "ca_mut.h"
#include "ca_api_proc.h"

#ifdef CONFIG_CA_NE_L2FP
#include "ca_l2fp.h"
#endif

//#ifdef CONFIG_CORTINA_BOARD_FPGA
#include "board_fpga.h"
//#else
//#error "No board definition?"
//#endif

static int bypass_fe = 0;

#if 0//sd1 uboot for 98f - TBD
MODULE_AUTHOR("Aaron Tseng, Raymond Tseng");
MODULE_DESCRIPTION(DRV_NAME);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_param(bypass_fe, int, 0444);
MODULE_PARM_DESC(bypass_fe, "All packets bypass Forwarding Engine");

static const u32 default_msg = NETIF_MSG_DRV | NETIF_MSG_PROBE
	| NETIF_MSG_LINK | NETIF_MSG_TIMER | NETIF_MSG_IFDOWN
	| NETIF_MSG_IFUP | NETIF_MSG_RX_ERR | NETIF_MSG_TX_ERR;

///TODO: merge ni_info_t to ca_eth_private_t
///      global data should be extract out or rename to be ni_global_data
ni_info_t ni_info_data;

///TODO: should all of global data be merged into ni_info_data???
#ifdef NI_RECYCLE_SKB
struct sk_buff_head ca_ni_skb_recycle_cpu0_head;
static int ni_min_skb_queue_len = RECYCLE_MIN_SKB_QUEUE_LEN;
#endif

///TODO: it seems that Linux kernel version 3.18 has removed skb recycle
//#define NI_RECYCLE_SKB			1
#endif

#if 1//yocto: aal_ni.c
#else//sd1 uboot for 98f
ca_uint8_t ca_ni_a53_cpu_port_count = CA_NI_TOTAL_CPU_PORT - 2;; /* 2 ports reserved for PEs */
#endif

#define RECYCLE_MIN_SKB_QUEUE_LEN	(8 * 1024)

/* debug flags */
u32 ca_ni_debug = 0;

static int debug = -1;  /* defaults above */

//static u16 linux_free_buf_size_tbl[CA_NI_TOTAL_CPU_PORT] = {CA_NI_TOTAL_EB_BUF_NUM, CA_NI_TOTAL_EB_BUF_NUM, CA_NI_TOTAL_EB_BUF_NUM, CA_NI_TOTAL_EB_BUF_NUM, 0, 0, 0, 0};

spinlock_t  ca_ni_rx_refill_lock;
static int ni_napi_budget = 16;
int ni_use_fbm = 0;
static int ni_initialized=0;
int ca_ni_mgmt_mode = 0;
int ca_ni_num_intf = 2;
char ca_ni_use_qm_eq_refill_int = 0;
char ca_ni_cpu64_pool0_eq_id = 0;
char ca_ni_cpu64_pool1_eq_id = 0;

void __iomem *ca_ne_sram_base = NULL;
static void __iomem *ca_ne_ddr_coherent_base = NULL;
static void __iomem *ca_ne_ddr_noncache_base = NULL;
static resource_size_t ca_ne_sram_phy_base = 0;
int ca_ne_sram_size = 0;
static resource_size_t ca_ne_ddr_noncache_phy_base = 0;
static resource_size_t ca_ne_ddr_coherent_phy_base = 0;
static ca_uint_t ca_ne_a53_pool0_start_addr, ca_ne_a53_pool1_start_addr;

#ifdef CONFIG_NET_SCH_MULTIQ
static u16 ca_ni_select_queue(struct net_device *dev, struct sk_buff *skb);
#endif

#ifdef CONFIG_CORTINA_MUT
extern struct kmem_cache *cache_16;	/* cache for 16-byte objects */
#endif

//static void ca_ni_cleanup_l3qm(void);
static int ca_ne_reg_init(struct platform_device *pdev);
void aal_ni_dma_lso_dump_registers(void);
void aal_l3qm_dump_registers(void);
int ca_ni_netlink_igmpsnooping_init(void);
void ca_ni_netlink_igmpsnooping_exit(void);
int  ca_ni_netlink_rx_init(void);
void  ca_ni_netlink_rx_exit(void);
void ca_ni_init_fbm(void);

/* index of interrupts in .dts file */
#define CA_NI_IRQ_QM_CPU_FIFO0		0
#define CA_NI_IRQ_QM_CPU_FIFO1		1
#define CA_NI_IRQ_QM_CPU_FIFO2		2
#define CA_NI_IRQ_QM_CPU_FIFO3		3
#define CA_NI_IRQ_QM_CPU_FIFO4		4
#define CA_NI_IRQ_QM_CPU_FIFO5		5
#define CA_NI_IRQ_QM_CPU_FIFO6		6
#define CA_NI_IRQ_QM_CPU_FIFO7		7
#define CA_NI_IRQ_QM_CPU_REFILL_REQ_8	8
#define CA_NI_IRQ_QM_CPU_REFILL_REQ_9	9

#if 0//sd1 uboot for 98f - TBD
/* export functions for kernel hook */
ca_status_t ca_ni_intf_get_ports(struct net_device *dev, ca_uint8_t *num_ports, int *ports, ca_uint8_t *is_wan_intf)
{
	int i;
	struct net_device *tmp_dev;
	ca_eth_private_t *cep = netdev_priv(dev);

	for (i = 0; i < ca_ni_num_intf; i++) {
		tmp_dev = ni_info_data.dev[i];

		if (tmp_dev == dev)
			break;;
	}
	if (i == ca_ni_num_intf) {
		printf("%s: dev->name=%s not found!!\n", __func__, dev->name);
		return CA_E_ERROR;
	}

	/* the first interface is allocated for WAN interface */
	printf("%s: dev->name=%s, i=%d\n", __func__, dev->name, i);
	if (i == 0)
		*is_wan_intf = 1;
	else {
		*is_wan_intf = 0;
	}

	if (cep->port_cfg.num_phy_port > *num_ports) {
		printf("%s: num_phy_port=%d bigger than num_ports=%d\n", __func__, cep->port_cfg.num_phy_port, *num_ports);
		return CA_E_PARAM;
	}
	*num_ports = cep->port_cfg.num_phy_port;
	for (i = 0; i < cep->port_cfg.num_phy_port; i++) {
		if (ca_ni_debug & NI_DBG_NI) {
			printf("%s: port_id[%d]=%d\n", __func__, i, cep->port_cfg.port_id[i]);
		}
		ports[i] = cep->port_cfg.port_id[i];
	}

	return CA_E_OK;
}
EXPORT_SYMBOL(ca_ni_intf_get_ports);

void ca_ni_alloc_main_hash(void)
{
	ca_uint32_t size;
	dma_addr_t main_hash_table_paddr, main_hash_action_paddr;
	ca_uint_t main_hash_table_saddr, main_hash_action_saddr;
	struct platform_device *pdev = ni_info_data.pdev;
	aal_l3qm_l3fe_main_hash_info_t l3fe_main_hash_info;

	aal_l3qm_get_l3fe_main_hash_action_info(&l3fe_main_hash_info);
	size = l3fe_main_hash_info.table_entry_count * l3fe_main_hash_info.table_entry_length;

	main_hash_table_saddr = dma_alloc_coherent(&(pdev->dev), size, &main_hash_table_paddr, GFP_KERNEL);
	printf("%s: Main Hash Table: main_hash_table_saddr=0x%p, main_hash_table_paddr=0x%p\n", __func__, main_hash_table_saddr, main_hash_table_paddr);

	size = l3fe_main_hash_info.action_table_entry_count * l3fe_main_hash_info.action_table_entry_length;
	main_hash_action_saddr = dma_alloc_coherent(&(pdev->dev), size, &main_hash_action_paddr, GFP_KERNEL);
	printf("%s: Main Hash Action: main_hash_action_saddr=0x%p, main_hash_action_paddr=0x%p\n", __func__, main_hash_action_saddr, main_hash_action_paddr);

	aal_l3qm_set_l3fe_main_hash(main_hash_table_saddr, main_hash_table_paddr, main_hash_action_saddr, main_hash_action_paddr);
}

#if !defined(CONFIG_ARCH_CORTINA_G3) && !defined(CONFIG_ARCH_CORTINA_G3HGU)
void aal_l3qm_set_a53_rx_fifo_addr(ca_uint_t a53_rx_fifo_phy_addr, ca_uint_t a53_rx_fifo_virt_addr);
ca_uint32_t aal_l3qm_get_a53_rx_fifo_size(void);

void ca_ni_alloc_a53_rx_fifo(void)
{
        ca_uint32_t size;
        ca_uint_t a53_rx_fifo_virt_addr;
        dma_addr_t a53_rx_fifo_phy_addr;
        struct platform_device *pdev = ni_info_data.pdev;

        size = aal_l3qm_get_a53_rx_fifo_size();
        printf("%s: aal_l3qm_get_a53_rx_fifo_size()=%d\n", __func__, size);

        a53_rx_fifo_virt_addr = dma_alloc_coherent(&(pdev->dev), size, &a53_rx_fifo_phy_addr, GFP_KERNEL);
        printf("%s: a53_rx_fifo_virt_addr=%p, a53_rx_fifo_phy_addr=%p\n", __func__, a53_rx_fifo_virt_addr, a53_rx_fifo_phy_addr);
        aal_l3qm_set_a53_rx_fifo_addr(a53_rx_fifo_phy_addr, a53_rx_fifo_virt_addr);
}
#endif

void ca_ni_init_reserved_mem(void)
{
	aal_l3qm_init_startup_config();
	aal_l3qm_init_reserved_mem(ca_ne_sram_base, ca_ne_sram_size, ca_ne_sram_phy_base, ca_ne_ddr_coherent_phy_base, ca_ne_ddr_noncache_base, ca_ne_ddr_noncache_phy_base);

#if !defined(CONFIG_ARCH_CORTINA_G3) && !defined(CONFIG_ARCH_CORTINA_G3HGU)
#if defined(CONFIG_ARCH_CORTINA_G3LITE)
	ca_ni_alloc_main_hash();
#endif
        ca_ni_alloc_a53_rx_fifo();
#endif

}


/***************************************************************************
 * netdev operations
 ***************************************************************************/

irqreturn_t ca_ni_qm_refill_interrupt(int irq, void *dev_instance)
{
	u16 eqid = 0;
	u32 bid_cnt;
	struct net_device *dev = (struct net_device *)dev_instance;
	//ca_eth_private_t *cep = netdev_priv((struct net_device *) dev_instance);

	if (irq == ni_info_data.refill_req_0) {
		eqid = ca_ni_cpu64_pool0_eq_id;
	}
	else if (irq == ni_info_data.refill_req_1) {
		eqid = ca_ni_cpu64_pool1_eq_id;
	}
	else {
		printf("%s: Unknown irq=%d!!!\n", __func__, irq);
	}

	aal_l3qm_enable_refill_threshold_interrupt(eqid, 0);

	bid_cnt = aal_l3qm_get_inactive_bid_cntr(eqid);

	//printf("%s: dev->name=%s, irq=%d, eqid=%d, bid_cnt=%d\n", __func__, dev->name, irq, eqid, bid_cnt);

	//if (bid_cnt > 30)
	ca_ni_refill_eq_buf_pool(dev, 0, eqid, bid_cnt);

	//udelay(1000);

	aal_l3qm_enable_refill_threshold_interrupt(eqid, 1);

	return IRQ_HANDLED;
}

/*
 * Handles RX interrupts.
 */
irqreturn_t ca_ni_rx_interrupt(int irq, void *dev_instance)
{
	int i = 0;
	u32 status;
	ca_eth_private_t *cep = netdev_priv((struct net_device *) dev_instance);
	unsigned long flags;
	int cpu_port;

	spin_lock_irqsave(&cep->lock, flags);

	if (ca_ni_mgmt_mode) {
		status = aal_ni_read_rx_interrupt_status(irq, ca_ni_mgmt_mode);

		if (ca_ni_debug & NI_DBG_NI) {
			printf("%s: interrupt status=0x%x\n", __func__, status);
		}

		aal_ni_write_rx_interrupt_status(irq, status, ca_ni_mgmt_mode);
	}

	//cpu_port = ca_ni_get_cpu_port_from_irq(cep, irq);
	cpu_port = cep->cpu_port;

	aal_ni_enable_rx_interrupt(cpu_port, 0, ca_ni_mgmt_mode);

	if (aal_ni_has_work(i, ca_ni_mgmt_mode)) {
		cep = netdev_priv((struct net_device *) dev_instance);
		napi_schedule(&cep->napi);
	}
	else {
		aal_ni_enable_rx_interrupt(cpu_port, 1, ca_ni_mgmt_mode);
	}

	spin_unlock_irqrestore(&cep->lock, flags);

	return IRQ_HANDLED;
}

static void ca_ni_set_flow_control(ca_port_cfg_info_t *port_cfg)
{
	int i;
	u8 pause_rx = 0, pause_tx = 0;
	ca_port_id_t port_id;

	if (port_cfg->flowctrl & CA_NI_RX_FLOW_CTRL)
		pause_rx = 1;

	if (port_cfg->flowctrl & CA_NI_TX_FLOW_CTRL)
		pause_tx = 1;

	for (i = 0; i < port_cfg->num_phy_port; i++) {
		port_id = ((ca_port_id_t)CA_PORT_TYPE_GPON << 8) | port_cfg->port_id[i];

		/* call Cortina port API */
		ca_eth_port_pause_set(0, port_id, 0, pause_rx, pause_tx);
	}
}

/* call back function for __kfree_skb() */
void ca_ni_free_skb_callback(struct sk_buff *skb)
{
#if 0
	if (aal_l3qm_eq_get_a53_use_fbm()) {
		aal_fbm_buf_push(FBM_CPU_ARM0, skb->fbm_pool_id, virt_to_phys(skb));
	}
#endif
}

#ifdef NI_RECYCLE_SKB
static int ca_ni_skb_recycle(struct sk_buff *skb)
{
	int ret = -1;

	if ((skb_queue_len(&ca_ni_skb_recycle_cpu0_head) < LINUX_FREE_BUF_LIST_SIZE) && skb_recycle_check(skb, SKB_PKT_LEN + 0x100)) {
		skb_queue_tail(&cs_ni_skb_recycle_cpu0_head, skb);
		ret = 0;
	}
	return ret;
}
#endif

/*
 * Prepare free buffers for TX/RX
 */
#ifdef NI_RECYCLE_SKB
static void ca_ni_prealloc_free_buffer(struct net_device *dev)
{
	int i;
	struct sk_buff *skb;
	ca_eth_private_t *cep = netdev_priv(dev);
	struct platform_device *pdev = cep->pdev;

	for (i = 0; i < ni_min_skb_queue_len; i++)
	{
		if (ni_min_skb_queue_len < skb_queue_len(&ca_ni_skb_recycle_cpu0_head))
			break;
		skb = ca_netdev_alloc_skb(dev, SKB_PKT_LEN + 0x100, true);
		if (!skb) {
			printf("%s: Not enough memory to allocate skb of size %d.\n", dev->name, SKB_PKT_LEN + 0x100);
			return -ENOMEM;
		}
		dma_map_single(&(pdev->dev), skb->data, SKB_PKT_LEN, DMA_FROM_DEVICE);
		skb_queue_tail(&ca_ni_skb_recycle_cpu0_head, skb);
	}
}
#endif

int ca_ni_init_l3qm(void)
{
	int i;

	/* make sure L3QM module is ready */
	aal_l3qm_check_init_done();

	/* disable RX of L3QM, because default it's enabled */
	aal_l3qm_enable_rx(0);

	aal_l3qm_init_empty_buffer(aal_l3qm_eq_get_a53_use_fbm());

	/* disable all of EPP interrupts */
	for (i = 0; i < 8; i++) {
		aal_l3qm_enable_cpu_epp_interrupt(i, 0);
	}

	aal_l3qm_init_cpu_epp();

	/* enable all of VoQs */
	aal_l3qm_init_voq();

	/* enable TX of L3QM */
	aal_l3qm_enable_tx(1);

	/* enable RX of L3QM */
	aal_l3qm_enable_rx(1);

	return 0;
}
//#endif

static void ca_ni_enable_ports(struct net_device *dev, ca_boolean_t enable)
{
	int i;
	ca_eth_private_t *cep = netdev_priv(dev);

	for (i = 0; i < cep->port_cfg.num_phy_port; i++) {
		ca_port_enable_set(0, ((ca_port_id_t)CA_PORT_TYPE_ETHERNET << CA_PORT_TYPE_SHIFT) |  cep->port_cfg.port_id[i], enable);
	}
}

static int ca_ni_request_qm_refill_irq(struct net_device *dev)
{
	char use_qm_refill_int;
	int retval;
	static int refill_irq = 0;
        char cpu64_pool0_eq_id = 0;
        char cpu64_pool1_eq_id = 0;

        /* read CFG_ID_NE_USE_QM_EQ_REFILL_INT from startup config */
        if (aal_scfg_read(CFG_ID_NE_USE_QM_EQ_REFILL_INT, 1, &use_qm_refill_int) == CA_E_OK) {
                ca_ni_use_qm_eq_refill_int = use_qm_refill_int;
                printf("%s: ca_ni_use_qm_eq_refill_int=%d\n", __func__, ca_ni_use_qm_eq_refill_int);
        }

        /* read eqid for CPU ports */
        if (aal_scfg_read(CFG_ID_L3QM_EQ_PROFILE_A53_POOL0_EQ_ID, 1, &cpu64_pool0_eq_id) == CA_E_OK) {
                ca_ni_cpu64_pool0_eq_id = cpu64_pool0_eq_id;
                printf("%s: ca_ni_cpu64_pool0_eq_id=%d\n", __func__, ca_ni_cpu64_pool0_eq_id);
        }
        if (aal_scfg_read(CFG_ID_L3QM_EQ_PROFILE_A53_POOL1_EQ_ID, 1, &cpu64_pool1_eq_id) == CA_E_OK) {
                ca_ni_cpu64_pool1_eq_id = cpu64_pool1_eq_id;
                printf("%s: ca_ni_cpu64_pool1_eq_id=%d\n", __func__, ca_ni_cpu64_pool1_eq_id);
        }

	if (refill_irq == 0 && ca_ni_use_qm_eq_refill_int) {

		retval = request_irq(ni_info_data.refill_req_0, ca_ni_qm_refill_interrupt, 0, "QM EQ REFILL0", dev);
		if (retval != 0 && retval != -EBUSY) {
			printf("%s: Unable to request IRQ %d. (errno %d)\n", __func__, ni_info_data.refill_req_0, retval);
			return retval;
		}

		retval = request_irq(ni_info_data.refill_req_1, ca_ni_qm_refill_interrupt, 0, "QM EQ REFILL1", dev);
		if (retval != 0 && retval != -EBUSY) {
			printf("%s: Unable to request IRQ %d. (errno %d)\n", __func__, ni_info_data.refill_req_1, retval);
			return retval;
		}
		refill_irq = 1;
	}
	return 0;
}

static int ca_ni_open(struct net_device *dev)
{
	int retval;
	ca_eth_private_t *cep = netdev_priv(dev);

	if (!ni_initialized) {
		printf("%s: dev->name=%s, probe not finished yet!!\n", __func__, dev->name);
		return 0;
	}

	/* should dirable RX interrupt first to avoid fake RX interrrupt comes in */
	aal_ni_enable_rx_interrupt(cep->cpu_port, 0, ca_ni_mgmt_mode);

#ifdef CONFIG_CA_NE_VIRTUAL_NI
	ca_eth_private_t *virt_cep = netdev_priv(dev);
	if (virt_cep->dev != dev)        /* virtual device now */
		return ca_ni_virt_ni_open(virt_cep->port_cfg.port_id, dev);
#endif

	netif_carrier_off(dev);

	/* request irq for this physical port */
	printf("%s: cep->port_cfg.irq=%d\n", __func__, cep->port_cfg.irq);

	retval = request_irq(cep->port_cfg.irq, ca_ni_rx_interrupt, 0, dev->name, dev);
	if (retval != 0 && retval != -EBUSY) {
		printf("%s: Unable to request IRQ %d. (errno %d)\n", __func__, cep->port_cfg.irq, retval);
		return retval;
	}

	/* reuqest QM REFILL INTERRUPTs */
	ca_ni_request_qm_refill_irq(dev);

	/* should enable EPP interrupt after request_irq() done */
	aal_l3qm_enable_cpu_epp_interrupt(cep->cpu_port, 1);

	setup_timer(&cep->tx_completion_timer, ca_ni_tx_completion_timer_cb, (unsigned long)dev);

	cep->status = 1;
	cep->ni_driver_state = 1;
	cep->tx_completion_queues = 0;

#ifdef NI_RECYCLE_SKB
	ca_ni_prealloc_free_buffer(dev);
#endif

	/* use Cortina API mdio */
//	if (cep->phydev) phy_start(cep->phydev);

	/* turn on TX/RX */
	ca_ni_enable_ports(dev, 1);

	/* set short term shaper on scheduler base on link speed */
	//TODO: Shaper is located at another module not NI???
	//ca_ni_set_short_term_shaper(tp);

	ca_ni_set_flow_control(&(cep->port_cfg));

	/* napi should be enabled before RX interrupt enabled */
	napi_enable(&cep->napi);

	/* Enable Rx interrupts at this point */
	//aal_ni_enable_rx_interrupt(cep->port_cfg.irq, 1, ca_ni_mgmt_mode);
	aal_ni_enable_rx_interrupt(cep->cpu_port, 1, ca_ni_mgmt_mode);

	/* call linux kernel routine to wake up all TX queues */
	netif_tx_wake_all_queues(dev);

	/* after wake all queues, should turn on carrier for ready to transmit */
	netif_carrier_on(dev);

	//napi_enable(&cep->napi);

#ifdef CONFIG_NET_SCH_MULTIQ
	/* multiple queues support */
	retval = netif_set_real_num_tx_queues(dev, 8);
	if (retval) {
		printf("%s: netif_set_real_num_tx_queues(), retval=%d\n", __func__, retval);
		return retval;
	}

	retval = netif_set_real_num_rx_queues(dev, 8);
	if (retval) {
		printf("%s: netif_set_real_num_rx_queues(), retval=%d\n", __func__, retval);
		return retval;
	}
#endif

	/* enable QM TX function for cpu port */
	aal_l3qm_enable_tx_cpu(cep->cpu_port, 1);

	return 0;
}

static int ca_ni_stop(struct net_device *dev)
{
	ca_eth_private_t *cep = netdev_priv(dev);

	/* disable QM TX function for cpu port */
	aal_l3qm_enable_tx_cpu(cep->cpu_port, 0);

	/* disable all of ports connected to this interface */
	ca_ni_enable_ports(dev, 0);

	aal_ni_enable_rx_interrupt(cep->cpu_port, 0, ca_ni_mgmt_mode);

	/* stop TX queues */
	netif_tx_disable(dev);

	/* disable NAPI */
	napi_disable(&cep->napi);

	/* release irq */
	free_irq(cep->port_cfg.irq, dev);

	cep->status = 0;

#ifdef NI_RECYCLE_SKB
	/* clean preallocate buffer */
	skb_queue_purge(&ca_ni_skb_recycle_cpu0_head);
#endif
	return 0;
}

static netdev_tx_t ca_ni_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	//int ldpid;
	int lso_txq_index;
	ca_eth_private_t *cep = netdev_priv(dev);
	ca_ni_tx_config_t tx_config;

	ca_skb_in(skb, true);

#ifdef CONFIG_NET_SCH_MULTIQ
	lso_txq_index = ca_ni_select_queue(dev, skb);
#else
	lso_txq_index = CA_NI_DMA_LSO_TXQ_IDX;
#endif

	/* should give ldpid instead of dvoq */
	if (ca_ni_mgmt_mode) {
		return ca_ni_mgmt_start_xmit(skb, dev, lso_txq_index, cep->port_cfg.tx_ldpid);
	}

	tx_config.core_config.longlong = 0;
	tx_config.core_config.bf.txq_index = lso_txq_index;
	tx_config.core_config.bf.ldpid = cep->port_cfg.tx_ldpid;

	return __ca_ni_start_xmit(skb, dev, &tx_config);
}

static int ca_ni_change_mtu(struct net_device *dev, int new_mtu)
{
	ca_eth_private_t *cep = netdev_priv(dev);

	/* validate new_mtu; if it is not validated, adjust it or return -EINVAL. */
	/* support jumbo frame */
	if (new_mtu < ETH_ZLEN || new_mtu > CA_NI_MAX_FRAME_SIZE)
		return -EINVAL;

	pr_info("%s: Change MTU from %d to %d.\n", dev->name, dev->mtu, new_mtu);
	dev->mtu = new_mtu;

	if (!netif_running(dev))
		return 0;

	/* restart TX queue */
	if (cep->phydev != NULL) {
		netif_tx_disable(dev);
		phy_stop(cep->phydev);
		phy_start(cep->phydev);
		netif_carrier_off(dev);
		netif_tx_wake_all_queues(dev);
	}

	return 0;
}

static int ca_ni_set_mac_address(struct net_device *dev, void *p)
{
	int i;
	struct sockaddr *addr = p;
	ca_eth_private_t *cep = netdev_priv(dev);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	if (ca_ni_mgmt_mode) {
		if (cep->port_cfg.num_phy_port > 1)
			return 0;
	}

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	for (i = 0; i < cep->port_cfg.num_phy_port; i++) {
		printf("%s: i=%d, cep->port_cfg.port_id[i]=%d\n", __func__, i, cep->port_cfg.port_id[i]);
		aal_ni_set_mac_address(cep->port_cfg.port_id[i], dev->dev_addr, ca_ni_mgmt_mode);
	}
	return 0;
}

static void ca_ni_tx_timeout(struct net_device *dev)
{
	int i;
	struct net_device *tmp_dev;
	ca_eth_private_t *cep = netdev_priv(dev);

	printf("%s: dev->name=%s\n", __func__, dev->name);

	for (i = 0; i < ca_ni_num_intf; i++) {
		tmp_dev = ni_info_data.dev[i];

		if (tmp_dev == NULL)
			continue;

		cep = netdev_priv(tmp_dev);
		netif_tx_stop_all_queues(tmp_dev);
	}

	///TODO: disable TX/RX interrupts???
	//ca_ni_disable_interrupts();

	printf("%s::schedule reset task\n", __func__);
	schedule_work(&ni_info_data.reset_task);
}

static int ca_ni_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		return ca_phy_ioctl(dev, ifr, cmd);
	default:
		return -EOPNOTSUPP;
	}
}

static void ca_ni_set_rx_mode(struct net_device *dev)
{
	///TODO: USe ACL to let the promiscuous mode take effect ???
#ifdef NI_USE_CPU_XRAM
	/* only for XRAM mode */
	if (dev->flags & IFF_PROMISC) {

		/// receive all packets

		pr_info("%s: Promiscuous mode is enabled.\n", dev->name);

	} else if (dev->flags & IFF_ALLMULTI) {

		/// receive all multicast

	} else {

		/// receive all broadcast

	}
#endif
}

static void ca_ni_change_rx_flags(struct net_device *dev, int flags)
{
	///TODO: Use ACL to let the promiscuous mode take effect ???
}

#define L4RxChkSum      (1 << 8)
static int ca_ni_set_features(struct net_device *dev, netdev_features_t features)
{
#if !defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
	ca_eth_private_t *cep = netdev_priv(dev);
	unsigned long flags;

	if ((dev->features ^ features) & NETIF_F_RXCSUM) {

		spin_lock_irqsave(&cep->lock, flags);

		if (features & NETIF_F_RXCSUM) {
			cep->rx_checksum = CA_ENABLE;
		} else {
			cep->rx_checksum = CA_DISABLE;
		}
		aal_l3fe_l4_chksum(cep->rx_checksum);

		spin_unlock_irqrestore(&cep->lock, flags);
	}
#endif
	return 0;
}

#if 0
static void ca_ni_conv_stats(struct net_device_stats *dev_stats, aal_ni_mib_stats_t *ni_stats)
{
	dev_stats->rx_packets += ni_stats->rx_uc_pkt_cnt + ni_stats->rx_mc_frame_cnt + ni_stats->rx_bc_frame_cnt;
	dev_stats->tx_packets += ni_stats->tx_uc_pkt_cnt + ni_stats->tx_mc_frame_cnt + ni_stats->tx_bc_frame_cnt;
#ifdef CONFIG_BIT64
	dev_stats->rx_bytes   += ((ca_uint_t)ni_stats->rx_byte_cnt_high << 32) + ni_stats->rx_byte_cnt_low;
	dev_stats->tx_bytes   += ((ca_uint_t)ni_stats->tx_byte_cnt_high << 32) + ni_stats->tx_byte_cnt_low;
#else
	dev_stats->rx_bytes   += ni_stats->rx_byte_cnt_low;
	dev_stats->tx_bytes   += ni_stats->tx_byte_cnt_low;
#endif
	dev_stats->rx_errors  += ni_stats->rx_crc_error_frame_cnt + ni_stats->rx_undersize_frame_cnt + ni_stats->rx_runt_frame_cnt +
		ni_stats->rx_ovsize_frame_cnt + ni_stats->rx_jabber_frame_cnt + ni_stats->rx_invalid_frame_cnt;
	dev_stats->tx_errors  += ni_stats->tx_crc_error_frame_cnt + ni_stats->tx_ovsize_frame_cnt + ni_stats->tx_single_col_frame_cnt +
		ni_stats->tx_multi_col_frame_cnt + ni_stats->tx_late_col_frame_cnt + ni_stats->tx_exess_col_frame_cnt;
	dev_stats->multicast  += ni_stats->rx_mc_frame_cnt;
	dev_stats->collisions += ni_stats->tx_single_col_frame_cnt + ni_stats->tx_multi_col_frame_cnt + ni_stats->tx_late_col_frame_cnt + ni_stats->tx_exess_col_frame_cnt;
	dev_stats->rx_length_errors += ni_stats->rx_undersize_frame_cnt + ni_stats->rx_runt_frame_cnt + ni_stats->rx_ovsize_frame_cnt;
	dev_stats->rx_frame_errors  += ni_stats->rx_invalid_frame_cnt;
	//dev_stats->tx_aborted_errors +=
	//dev_stats->tx_carrier_errors +=
	dev_stats->rx_crc_errors += ni_stats->rx_crc_error_frame_cnt;
	//dev_stats->rx_missed_errors =
}
#endif

static struct net_device_stats *ca_ni_get_stats(struct net_device *dev)
{
	//int i;
	//aal_ni_mib_stats_t	ni_stats;
	//ca_eth_private_t *cep = netdev_priv(dev);

	///TODO: use software counter in stead of hardware counter now
#if 0
	memset(&ni_stats, 0, sizeof(aal_ni_mib_stats_t));
	for (i = 0; i < cep->port_cfg.num_phy_port; i++) {
		aal_ni_eth_port_mib_get(0, cep->port_cfg.port_id[i], 0, &ni_stats);
		ca_ni_conv_stats(&dev->stats, &ni_stats);
	}
#endif

	//dev->stats->rx_dropped = cep->rx_dropped;
	//dev->stats->tx_dropped = cep->tx_dropped;
	return &dev->stats;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void ca_ni_poll_controller(struct net_device *dev)
{
	ca_eth_private_t *cep = netdev_priv(dev);

	disable_irq(dev->irq);

	/// call interrupt handler to handle packets; ex. ni_rx_irq_handler(dev->irq, dev);

	ca_ni_rx_interrupt(cep->port_cfg.irq, dev);

	enable_irq(dev->irq);
}
#endif /* CONFIG_NET_POLL_CONTROLLER */

#ifdef CONFIG_NET_SCH_MULTIQ
#define QUEUE_PER_INSTANCE  8
static u16 ca_ni_select_queue(struct net_device *dev, struct sk_buff *skb)
{
	u16 queue_id;

	/* Get the queue id from skb->priority here
	 * It will update real queue id before create hash if
	 * tc change the queue mapping.
	 */
	///TODO: qid should be selected??
	queue_id = QUEUE_PER_INSTANCE - (skb->priority & 0x7) - 1;
	//queue_id = 0;

	printf("%s: queue_id=%d\n", __func__, queue_id);
	return queue_id;
}
#endif /* CONFIG_NET_SCH_MULTIQ */

#if 0//yocto
static const struct net_device_ops ca_ni_netdev_ops = {
	.ndo_open		= ca_ni_open,
	.ndo_stop		= ca_ni_stop,
	.ndo_start_xmit		= ca_ni_start_xmit,
	.ndo_tx_timeout		= ca_ni_tx_timeout,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_change_mtu		= ca_ni_change_mtu,
	.ndo_set_mac_address	= ca_ni_set_mac_address,
	.ndo_do_ioctl		= ca_ni_ioctl,
	.ndo_set_rx_mode	= ca_ni_set_rx_mode,
	.ndo_change_rx_flags	= ca_ni_change_rx_flags,
	.ndo_set_features	= ca_ni_set_features,
	.ndo_get_stats		= ca_ni_get_stats,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= ca_ni_poll_controller,
#endif
#ifdef CONFIG_NET_SCH_MULTIQ
#ifndef CONFIG_XPS
	/* In XPS mode, no need to select queue here */
	/* use "echo 2 > /sys/class/net/eth0/tx-4/xps_cpus" to bind TX queue 4 to CPU 1 */
	.ndo_select_queue	= ca_ni_select_queue,
#endif
#endif
};
#else//sd1 uboot - TBD
#endif

/***************************************************************************
 * platform driver registration
 ***************************************************************************/

#define CA_NI_SCFG_READ_INTF_PORTS(idx, data) aal_scfg_read(CFG_ID_NE_INTF_PORTS##idx, CFG_ID_NE_INTF_PORTS_LEN, (ca_uint8_t *)data)
#define CA_NI_IRQ_OF_PARSE_AND_MAP(np, idx) irq_of_parse_and_map(np, CA_NI_IRQ_QM_CPU_FIFO##idx)
#define CA_NI_SCFG_READ_MAC_ADDR(idx, mac_addr) aal_scfg_read(CFG_ID_NE_MAC_ADDR##idx, CA_ETH_ADDR_LEN, mac_addr);

static void ca_ni_dump_intfs(void)
{
	int i, j;
	ca_eth_private_t *cep;

	for (i = 0; i < ca_ni_num_intf; i++) {
		cep = netdev_priv(ni_info_data.dev[i]);
		printf("%s: ===================== interface %d ===================\n", __func__, i);
		printf("cpu_port=%d\n", cep->cpu_port);
		for (j = 0; j < port_cfg_infos[i].num_phy_port; j++) {
			printf("port_id[%d]=%d\n", j, port_cfg_infos[i].port_id[j]);
		}
		printf("MAC ADDR=%pM\n", port_cfg_infos[i].mac_addr);
		printf("duplex=0x%x, flowctrl=0x%x, irq=%d, rx_ldpid=%d, tx_ldpid=%d\n",
			port_cfg_infos[i].duplex, port_cfg_infos[i].flowctrl, port_cfg_infos[i].irq,
			port_cfg_infos[i].rx_ldpid, port_cfg_infos[i].tx_ldpid);
	}
}

static void ca_ni_init_port_configs(struct platform_device *pdev)
{
	int i, j, cnt, ret, ret1, irq;
	ca_uint8_t ne_intf_ports[CFG_ID_NE_INTF_PORTS_LEN];
	ca_uint8_t ne_rx_ldpids[CFG_ID_NE_RX_LDPID_LEN];
	ca_uint8_t ne_tx_ldpids[CFG_ID_NE_TX_LDPID_LEN];
	ca_mac_addr_t mac_addr;
#ifdef CONFIG_OF
	struct device_node *np = pdev->dev.of_node;
#endif


	if (aal_scfg_read(CFG_ID_NE_RX_LDPID, CFG_ID_NE_RX_LDPID_LEN, ne_rx_ldpids) != CA_E_OK) {
		printf("%s: RX LDPID for all interface does not be specified!!\n", __func__);
		BUG();
	}
	if (aal_scfg_read(CFG_ID_NE_TX_LDPID, CFG_ID_NE_TX_LDPID_LEN, ne_tx_ldpids) != CA_E_OK) {
		printf("%s: TX LDPID for all interface does not be specified!!\n", __func__);
		BUG();
	}

	/* get the required constants for all interfaces from startup config */
	for (i = 0; i < CA_NI_MAX_INTF; i++) {
		memset(&(port_cfg_infos[i]), 0, sizeof(ca_port_cfg_info_t));
		switch (i) {
			case 0:
				ret = CA_NI_SCFG_READ_INTF_PORTS(0, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 0);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(0, mac_addr);
				break;
			case 1:
				ret = CA_NI_SCFG_READ_INTF_PORTS(1, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 1);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(1, mac_addr);
				break;
			case 2:
				ret = CA_NI_SCFG_READ_INTF_PORTS(2, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 2);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(2, mac_addr);
				break;
			case 3:
				ret = CA_NI_SCFG_READ_INTF_PORTS(3, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 3);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(3, mac_addr);
				break;
			case 4:
				ret = CA_NI_SCFG_READ_INTF_PORTS(4, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 4);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(4, mac_addr);
				break;
			case 5:
				ret = CA_NI_SCFG_READ_INTF_PORTS(5, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 5);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(5, mac_addr);
				break;
			case 6:
				ret = CA_NI_SCFG_READ_INTF_PORTS(6, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 6);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(6, mac_addr);
				break;
			case 7:
				ret = CA_NI_SCFG_READ_INTF_PORTS(7, ne_intf_ports);
				irq = CA_NI_IRQ_OF_PARSE_AND_MAP(np, 7);
				ret1 = CA_NI_SCFG_READ_MAC_ADDR(7, mac_addr);
				break;
			default:
				printf("%s: interface ID exceed 7!!\n", __func__);
				BUG();
				break;
		}

		if (ret == CA_E_OK && ret1 == CA_E_OK) {
			for (j = 0, cnt = 0;  j < CFG_ID_NE_INTF_PORTS_LEN && cnt < CA_NI_NUM_OF_PORT; j++) {
				if (ne_intf_ports[j] == 0xFF)
					continue;
				port_cfg_infos[i].port_id[cnt] = ne_intf_ports[j];
				cnt++;
			}
			port_cfg_infos[i].num_phy_port = cnt;
			port_cfg_infos[i].duplex = DUPLEX_FULL,
			port_cfg_infos[i].flowctrl = FLOW_CTRL_TX | FLOW_CTRL_RX,
			port_cfg_infos[i].irq = irq;            /* irq will be got from device tree later */
			port_cfg_infos[i].rmii_clk_src = 0;
			memcpy(&(ca_ni_eth_mac[i][0]), mac_addr, CA_ETH_ADDR_LEN);
			port_cfg_infos[i].mac_addr = &(ca_ni_eth_mac[i][0]);
			port_cfg_infos[i].rx_ldpid = ne_rx_ldpids[i];
			port_cfg_infos[i].tx_ldpid = ne_tx_ldpids[i];
		}
		else {
			if (ret != CA_E_OK)
				printf("%s: interface ID %d does not be defined in startup config!!\n", __func__, i);
			if (ret1 != CA_E_OK)
				printf("%s: interface ID %d does not be defined in startup config!!\n", __func__, i);
			BUG();
		}
	}
}

static int ca_ni_init_cpu_ports(struct platform_device *pdev)
{
	int i, cnt;
	struct net_device *dev;
	ca_port_cfg_info_t *port_cfg;
	ca_eth_private_t *cep;
	struct device_node *np = pdev->dev.of_node;
	aal_ni_hv_glb_internal_port_id_cfg_t ni_port_id_cfg;

	ca_ni_init_port_configs(pdev);

	/* dump port config for all of interfaces */
	//ca_ni_dump_intfs();
	aal_ni_hv_glb_internal_port_id_cfg_get(0, &ni_port_id_cfg);

	/* by default there is only 2 network interface */
	/* LANL2RP0 lpid=16 used for LAN port */
	/* WANL2RP0 lpid=17 used for WAN port */
	for (i = 0; i < ca_ni_num_intf; i++) {

		port_cfg = &port_cfg_infos[i];
#ifdef CONFIG_NET_SCH_MULTIQ
		dev = alloc_etherdev_mq(sizeof(ca_eth_private_t), QUEUE_PER_INSTANCE);
#else
		dev = alloc_etherdev(sizeof(ca_eth_private_t));
#endif
		if (dev == NULL) {
			pr_err("Unable to alloc new ethernet interface #%d.\n", i);
			return -ENOMEM;
		}

		/* Set the sysfs physical device reference for the network logical device */
		SET_NETDEV_DEV(dev, &pdev->dev);

		ni_info_data.dev[i] = dev;

		/* fill all of the items of cep */
		cep = netdev_priv(dev);
		cep->dev = dev;
		cep->pdev = pdev;
		cep->port_cfg = *port_cfg;
#if !defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
		cep->rx_checksum = CA_ENABLE;
#else
		/* in Saturn SFU the packets come from L2FE no hardware checksum supportted */
		cep->rx_checksum = CA_DISABLE;
#endif
		cep->an_bng_mac_idx = 0xffff;
		cep->an_bng_mac_idx_pe0 = 0xffff;
		cep->an_bng_mac_idx_pe1 = 0xffff;

		/* add a cpu_port for recognize */
		cep->cpu_port = i;

		/* msg_enable can be get/set by ethtool for debug message level */
		cep->msg_enable = netif_msg_init(debug, default_msg);

		cep->napi.dev = dev;
		cep->use_default_lso_para0 = 0;
		cep->deep_q = 0;
		cep->dma_l3fe = (ni_port_id_cfg.dma_to_l3fe & (1 << i))? 1:0;

		///TODO: should these configuration for NI ports be done in Cortina port APIs ???
		memcpy(&dev->dev_addr[0], &(cep->port_cfg.mac_addr[0]), 6);
		memcpy(dev->perm_addr, dev->dev_addr, 6);

		/* in mgmt mode, only port 1 can be set mac address */
		if (ca_ni_mgmt_mode && i == 0) {
			aal_ni_set_mac_address(cep->port_cfg.port_id[0], dev->dev_addr, ca_ni_mgmt_mode);
		}
		else {
			int j;
			for (j = 0; j < cep->port_cfg.num_phy_port; j++) {
				aal_ni_set_mac_address(cep->port_cfg.port_id[j], dev->dev_addr, ca_ni_mgmt_mode);
			}
		}

		/* initialize TX/RX flow control */
		ca_ni_set_flow_control(&(cep->port_cfg));

		/* fill all of the items of dev */
		dev->netdev_ops = &ca_ni_netdev_ops;
		ca_ni_set_ethtool_ops(dev);

		if (ca_ni_mgmt_mode)
			dev->features = NETIF_F_SG | NETIF_F_RXCSUM | NETIF_F_UFO;
		else
			dev->features = NETIF_F_SG | NETIF_F_HW_CSUM | NETIF_F_RXCSUM | NETIF_F_UFO;

		dev->hw_features = dev->features | NETIF_F_TSO | NETIF_F_TSO6 ;
		///   dev->vlan_features
		dev->irq = cep->port_cfg.irq;
		dev->watchdog_timeo = CA_NI_TX_TIMEOUT;
		dev->features |= NETIF_F_GRO;

		/* initialize spin locks */
		spin_lock_init(&cep->stats_lock);
		spin_lock_init(&cep->lock);
	} /* for (i = 0; i < ca_ni_num_intf; i++) */

	/* dump port config for all of interfaces */
	ca_ni_dump_intfs();
	return 0;
}

enum dma_data_direction {
	DMA_BIDIRECTIONAL	= 0,
	DMA_TO_DEVICE		= 1,
	DMA_FROM_DEVICE		= 2,
};

static u32 ca_ni_fill_empty_buffer_by_rule(struct net_device *dev, u8 eqid)
{
	u32 phy_addr = 0;
	u8 a53_pool0_eq_id, a53_pool1_eq_id;
	struct sk_buff *skb;
	ca_eth_private_t *cep = netdev_priv(dev);
	struct platform_device *pdev = cep->pdev;
	int head_room = aal_l3qm_get_cpu_port_head_room_first();
	ca_uint_t a53_pool0_start_addr, a53_pool1_start_addr;

	if (aal_l3qm_eq_get_a53_rule() == 0) {

		/* allocate skb data size=2048+32(head room)+16(reset room) */
		skb = ca_netdev_alloc_skb(dev, CA_NI_SKB_PKT_LEN + head_room + 16, true);

		if (!skb) {
			pr_warning("%s: ca_netdev_alloc_skb() failed!!!\n", dev->name);
			return 0;
		}

                /* add some checking for debuggig */
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
		if (ca_ni_debug & NI_DBG_NI) {
			printf("%s: skb->data=0x%p\n", __func__, skb->data);
		}
		if ((unsigned long)(skb->data) % 128) {
			printf("%s: skb->data=0x%p not alignment!!\n", __func__, skb->data);
		}
#endif

		/* store the sk_buff pointer to software head room */
		*((ca_uint_t *)(skb->data)) = (ca_uint_t)skb;

		/* flush the data we write to skb->data to prevent later CPU flush cache data to overwrite the data filled by hardware */
		dma_map_single(&(pdev->dev), skb->data, 128, DMA_TO_DEVICE);

		/* add nop for debuggig */
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
		dma_wmb();
#endif

		/* should invalidate the cache data to prevent the wrong data be written to device when the cache is swapped out */
		dma_map_single(&(pdev->dev), skb->data, CA_NI_SKB_PKT_LEN + head_room + 16, DMA_FROM_DEVICE);

                /* add nop for debuggig */
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
		dma_wmb();
#endif

		/* an private flag to indicate the skb buffer is allocated for L3QM */
		skb->is_from_l3qm = 1;

		phy_addr = virt_to_phys(skb->data);

		if (ca_ni_debug & NI_DBG_NI) {
			printf("%s: phy_addr=0x%x, skb->head=%p, skb->data=%p, skb=%p, head_stored=0x%llx\n",
					__func__, phy_addr, skb->head, skb->data, skb, *((ca_uint64_t *)(skb->data)));
		}
	}
	else {
		aal_l3qm_eq_get_a53_pool0_1_eq_id(&a53_pool0_eq_id, &a53_pool1_eq_id);
		if (eqid == a53_pool0_eq_id) {
			phy_addr = virt_to_phys((void *) ca_ne_a53_pool0_start_addr);

			ca_ne_a53_pool0_start_addr += CA_NI_SKB_PKT_LEN;
			aal_l3qm_eq_get_a53_pool0_1_start_addr(&a53_pool0_start_addr, &a53_pool1_start_addr);
			if (ca_ne_a53_pool0_start_addr > (a53_pool0_start_addr + CA_NI_SKB_PKT_LEN * aal_l3qm_eq_get_a53_pool_bid_count())) {
				printf("%s: ca_ne_a53_pool0_start_addr=0x%llx exceed boundary\n", __func__, (ca_uint64_t)ca_ne_a53_pool0_start_addr);
				ca_ne_a53_pool0_start_addr = a53_pool0_start_addr;
			}
		}
		if (eqid == a53_pool1_eq_id) {
			phy_addr = virt_to_phys((void *) ca_ne_a53_pool1_start_addr);

			ca_ne_a53_pool1_start_addr += CA_NI_SKB_PKT_LEN;
			aal_l3qm_eq_get_a53_pool0_1_start_addr(&a53_pool0_start_addr, &a53_pool1_start_addr);
			if (ca_ne_a53_pool1_start_addr > (a53_pool1_start_addr + CA_NI_SKB_PKT_LEN * aal_l3qm_eq_get_a53_pool_bid_count())) {
				printf("%s: ca_ne_a53_pool1_start_addr=0x%llx exceed boundary\n", __func__, (ca_uint64_t)ca_ne_a53_pool1_start_addr);
				ca_ne_a53_pool1_start_addr = a53_pool1_start_addr;
			}
		}
		if (ca_ni_debug & NI_DBG_NI) {
			printf("%s: eqid=%d, a53_pool0_eq_id=%d, a53_pool1_eq_id=%d\n", __func__, eqid, a53_pool0_eq_id, a53_pool1_eq_id);
			printf("%s: phy_addr=0x%x, ca_ne_a53_pool0_start_addr=0x%llx, ca_ne_a53_pool1_start_addr=0x%llx\n",
				__func__, phy_addr, (ca_uint64_t)ca_ne_a53_pool0_start_addr, (ca_uint64_t)ca_ne_a53_pool1_start_addr);
		}
	}
	return phy_addr;
}

static void ca_ni_fill_empty_buffer(struct net_device *dev, u8 cpu_port, u8 eqid, u16 inactive_bid_cntr)
{
	int i;
	u16 count;
	u32 phy_addr = 0;
	int push_count = 0;

	if (ca_ni_debug & NI_DBG_NI) {
		printf("%s: cpu_port=%d, eqid=%d, inactive_bid_cntr=%d\n", __func__, cpu_port, eqid, inactive_bid_cntr);
	}

	while (inactive_bid_cntr) {

		if (inactive_bid_cntr > 8) {
			count = 8;
			inactive_bid_cntr -= 8;
		}
		else {
			count = inactive_bid_cntr;
			inactive_bid_cntr = 0;
		}

		/* push physical address to Empty Buffer */
		for (i = 0; i < count; i++) {

			phy_addr = ca_ni_fill_empty_buffer_by_rule(dev, eqid);
			if (phy_addr == 0) continue;

			if (aal_l3qm_eq_get_a53_use_fbm()) {
				//ca_uint32_t phy_start_addr, entry_size, entry_count;
				//ca_uint8_t fbm_pool_id;

				///only for testing, skb data should be pushed to  pool1
				///has been pushed when init
#if 0
				aal_l3qm_eq_get_a53_pool1_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
				aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_addr);
				skb->fbm_pool_id = fbm_pool_id;
#endif
			}
			else {
				/* check ready bit */
				if (aal_l3qm_check_cpu_push_ready(cpu_port)) {
					aal_l3qm_set_cpu_push_paddr(cpu_port, eqid, phy_addr);
					push_count++;
				} else {
					/*Need to free the packet*/
					if (aal_l3qm_eq_get_a53_rule() == 0) {
						struct sk_buff *skb = (struct sk_buff *) (*((ca_uint_t *) phys_to_virt(phy_addr)));
						ca_dev_kfree_skb_any(skb);

						if (ca_ni_debug & NI_DBG_NI) {
							printf("%s: fill buffer fail at cpu_port[%d] eqid[%d]\n",
								__func__, cpu_port, eqid);
						}
					}
				}
			}
		} /* for loop */

		/* only check req bit, do not update inactive_brd_cntr to avoid infinite loop */
		if (aal_l3qm_get_inactive_bid_cntr(eqid) == 0) {
			//printf("%s: req bit not 1 or inactive bid count=0!!\n", __func__);
			break;
		}

	} /* while loop */

	if (ca_ni_debug & NI_DBG_NI) {
		printf("%s: ======== push_count=%d ======== \n", __func__, push_count);
	}
}

void ca_ni_refill_eq_buf_pool(struct net_device *dev, u8 cpu_port, u8 eqid, int cnt)
{
	u16 inactive_bid_cntr;
	unsigned long flags;

	/* the EQ pools are shared by all of A53 CPU ports should be locked before changed */
	//spin_lock_bh(&ca_ni_rx_refill_lock);
	spin_lock_irqsave(&ca_ni_rx_refill_lock, flags);

	/* refill the first EQ buffer pool */
	inactive_bid_cntr = aal_l3qm_get_inactive_bid_cntr(eqid);
	if (inactive_bid_cntr > cnt)
		inactive_bid_cntr = cnt;
	if (inactive_bid_cntr)
		ca_ni_fill_empty_buffer(dev, cpu_port, eqid, inactive_bid_cntr);

	//spin_unlock_bh(&ca_ni_rx_refill_lock);
	spin_unlock_irqrestore(&ca_ni_rx_refill_lock, flags);

	if (ca_ni_debug & NI_DBG_NI)
		printf("%s: ======== cpu_port=%d, eqid=%d, inactive_bid_cntr=%d, cnt=%d ======== \n", __func__, cpu_port, eqid, inactive_bid_cntr, cnt);
}

void ca_ni_fill_eq_buf_pool(struct net_device *dev, u8 cpu_port, int cnt)
{
	/* fill EQ buffer for CPU64 */
	ca_ni_refill_eq_buf_pool(dev, cpu_port, l3qm_eq_profile_a53_pool0_eq_id, cnt);
	ca_ni_refill_eq_buf_pool(dev, cpu_port, l3qm_eq_profile_a53_pool1_eq_id, cnt);
}

///for header=SRAM data=DDR Tx/RX test
void ca_ni_init_fbm(void)
{
	int i;
	ca_uint32_t phy_start_addr, entry_size, entry_count;
	ca_uint_t virt_start_addr;
	ca_uint8_t fbm_pool_id;
	struct aal_fbm_pool_config_s  fbm_pool;

	aal_fbm_init();

	if (aal_l3qm_eq_get_a53_use_fbm()) {

		aal_l3qm_eq_get_a53_pool0_info(&virt_start_addr, &entry_size, &entry_count, &fbm_pool_id);

		phy_start_addr = virt_to_phys((void *) virt_start_addr);

		printf("%s: A53 pool0 virt_start_addr=0x%llx, phy_start_addr=0x%x, entry_size=%d, entry_count=%d, fbm_pool_id=%d\n", __func__,
				(ca_uint64_t)virt_start_addr, phy_start_addr, entry_size, entry_count, fbm_pool_id);

		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		for (i = 0; i < entry_count; i++) {

			if (ca_ni_debug & NI_DBG_NI) {
				printf("%s: A53 pool 0 pushed physical address = 0x%x\n", __func__, phy_start_addr + (i * entry_size));
			}

			aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_start_addr + (i * entry_size));
		}

		aal_l3qm_eq_get_a53_pool1_info(&virt_start_addr, &entry_size, &entry_count, &fbm_pool_id);

		phy_start_addr = virt_to_phys((void *) virt_start_addr);

		printf("%s: A53 pool1 virt_start_addr=0x%llx, phy_start_addr=0x%x, entry_size=%d, entry_count=%d, fbm_pool_id=%d\n", __func__,
				(ca_uint64_t)virt_start_addr, phy_start_addr, entry_size, entry_count, fbm_pool_id);

		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		for (i = 0; i < entry_count; i++) {

			if (ca_ni_debug & NI_DBG_NI) {
				printf("%s: A53 pool 1 pushed physical address = 0x%x\n", __func__, phy_start_addr + (i * entry_size));
			}

			aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_start_addr + (i * entry_size));
		}
	}

#if 0
	/* initialize FBM pool for PE0 and PE1 */
	if (aal_l3qm_eq_get_pe0_use_fbm()) {
		aal_l3qm_eq_get_pe0_pool0_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		aal_l3qm_eq_get_pe0_pool1_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);
	}

	if (aal_l3qm_eq_get_pe1_use_fbm()) {
		aal_l3qm_eq_get_pe1_pool0_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		aal_l3qm_eq_get_pe1_pool1_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);
	}
#endif

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	if (aal_l3qm_eq_get_cpu256_hi_use_fbm()) {
		aal_l3qm_eq_get_cpu256_hi_pool0_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		printf("%s: CPU256 hi pool 0 phy_start_addr=0x%x, entry_size=%d, entry_count=%d, fbm_pool_id=%d\n", __func__,
		phy_start_addr, entry_size, entry_count, fbm_pool_id);

		/* PE will do the FBM push */
#if 0
		for (i = 0; i < entry_count; i++) {

			if (ca_ni_debug & NI_DBG_NI) {
				printf("%s: CPU256 hi pool 0 pushed physical address = 0x%x\n", __func__, phy_start_addr + (i * entry_size));
			}

			aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_start_addr + (i * entry_size));
		}

		aal_l3qm_eq_get_cpu256_hi_pool1_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		//aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		printf("%s: CPU256 hi pool 1 phy_start_addr=0x%x, entry_size=%d, entry_count=%d, fbm_pool_id=%d\n", __func__,
			phy_start_addr, entry_size, entry_count, fbm_pool_id);

		for (i = 0; i < entry_count; i++) {

			if (ca_ni_debug & NI_DBG_NI) {
				printf("%s: CPU256 hi pool 1 pushed physical address = 0x%x\n", __func__, phy_start_addr + (i * entry_size));
			}

			aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_start_addr + (i * entry_size));
		}
#endif
	}

	if (aal_l3qm_eq_get_cpu256_lo_use_fbm()) {
		aal_l3qm_eq_get_cpu256_lo_pool0_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		printf("%s: CPU256 lo pool 0 phy_start_addr=0x%x, entry_size=%d, entry_count=%d, fbm_pool_id=%d\n", __func__,
			phy_start_addr, entry_size, entry_count, fbm_pool_id);

		printf("%s: inactive BID counter=%d\n", __func__, aal_l3qm_get_inactive_bid_cntr(6));

		/* PE will do the FBM push */
#if 0
		for (i = 0; i < entry_count; i++) {

			if (ca_ni_debug & NI_DBG_NI) {
				printf("%s: CPU256 lo pool 0 pushed physical address = 0x%x\n", __func__, phy_start_addr + (i * entry_size));
			}

			aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_start_addr + (i * entry_size));
		}

		aal_l3qm_eq_get_cpu256_lo_pool1_info(&phy_start_addr, &entry_size, &entry_count, &fbm_pool_id);
		fbm_pool.cache_invalidate = 0;
		fbm_pool.memset_mode = FBM_MSET_DISABLE;
		fbm_pool.mset_cainv_size = entry_size;
		//aal_fbm_pool_init(fbm_pool_id, &fbm_pool);

		printf("%s: CPU256 lo pool 1 phy_start_addr=0x%x, entry_size=%d, entry_count=%d, fbm_pool_id=%d\n", __func__,
			phy_start_addr, entry_size, entry_count, fbm_pool_id);

		printf("%s: inactive BID counter=%d\n", __func__, aal_l3qm_get_inactive_bid_cntr(7));
		for (i = 0; i < entry_count; i++) {

			if (ca_ni_debug & NI_DBG_NI) {
				printf("%s: CPU256 lo pool 1 pushed physical address = 0x%x\n", __func__, phy_start_addr + (i * entry_size));
			}

			aal_fbm_buf_push(FBM_CPU_ARM0, fbm_pool_id, phy_start_addr + (i * entry_size));
		}
#endif
	}
#endif
}

static void ca_ni_reset_task(struct work_struct *work)
{
	int i;
	ca_eth_private_t *cep;
	struct net_device *dev;

	printf("%s \n" , __func__);

	rtnl_lock();

	for (i = 0; i < ca_ni_num_intf; i++) {
		dev = ni_info_data.dev[i];
		if (dev == NULL)
			continue;
		cep = netdev_priv(dev);
		napi_disable(&cep->napi);
	}

	cancel_tx_completion_timer(dev);

	// reset tx ring
	ca_ni_reset_tx_ring();

	for (i = 0; i < ca_ni_num_intf; i++) {
		dev = ni_info_data.dev[i];
		if (dev == NULL)
			continue;
		cep = netdev_priv(dev);
		netif_tx_wake_all_queues(dev);  // enable tx
	}

	for (i = 0; i < ca_ni_num_intf; i++) {
		dev = ni_info_data.dev[i];
		if (dev == NULL)
			continue;
		cep = netdev_priv(dev);
		napi_enable(&cep->napi);
	}

	//ca_ni_enable_interrupts();

	rtnl_unlock();
} /* cs_ni_reset_task */

/* NAPI polling routine for CPU port 0 (WAN port) */
static int ca_ni_poll_0(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT0_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 1 (LAN port) */
static int ca_ni_poll_1(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT1_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 2 (LAN port) */
static int ca_ni_poll_2(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT2_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 3 (LAN port) */
static int ca_ni_poll_3(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT3_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 4 (LAN port) */
static int ca_ni_poll_4(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT4_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 5 (LAN port) */
static int ca_ni_poll_5(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT5_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 6 (LAN port) */
static int ca_ni_poll_6(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT6_INST, napi, budget, ca_ni_mgmt_mode);
}

/* NAPI polling routine for CPU port 7 (LAN port) */
static int ca_ni_poll_7(struct napi_struct *napi, int budget)
{
	return ca_ni_rx_poll(CPU_PORT7_INST, napi, budget, ca_ni_mgmt_mode);
}

static int ca_ni_register_netdev(void)
{
	int i, err;

	for (i = 0; i < ca_ni_num_intf; i++) {
		err = register_netdev(ni_info_data.dev[i]);
		if (err) {
			printf("%s: Cannot register net device, aborting.\n", ni_info_data.dev[i]->name);
			free_netdev(ni_info_data.dev[i]);
			return err;
		}
	}
	return 0;
}

#ifdef CONFIG_OF
static void ca_ni_global_reset(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct reset_control *ni_reset, *l2fe_reset, *l2tm_reset, *l3fe_reset, *tqm_reset, *dma_reset;
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	ca_uint8_t phy_intfs[CFG_ID_PHY_INTERFACE_LEN];
	struct phy *sgmii_phy;
	struct reset_control *phy_sdsif_reset, *phy_sgmii_reset;
#endif
#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU) || defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	 struct reset_control *ext_reset;
#endif
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
	struct device_node *saturn_np;
	struct reset_control *saturn_reset;
#endif

	ni_reset = of_reset_control_get(np, "ni_reset");
	if (IS_ERR(ni_reset)) {
		dev_warn(&pdev->dev, "OF get ni_reset failed!!");
	}
	else {
		reset_control_assert(ni_reset);
		udelay(1);
		reset_control_deassert(ni_reset);
	}
	reset_control_put(ni_reset);

	l2fe_reset = of_reset_control_get(np, "l2fe_reset");
	if (IS_ERR(l2fe_reset)) {
		dev_warn(&pdev->dev, "OF get l2fe_reset failed!!");
	}
	else {
		reset_control_assert(l2fe_reset);
		udelay(1);
		reset_control_deassert(l2fe_reset);
	}
	reset_control_put(l2fe_reset);

	l2tm_reset = of_reset_control_get(np, "l2tm_reset");
	if (IS_ERR(l2tm_reset)) {
		dev_warn(&pdev->dev, "OF get l2tm_reset failed!!");
	}
	else {
		reset_control_assert(l2tm_reset);
		udelay(1);
		reset_control_deassert(l2tm_reset);
	}
	reset_control_put(l2tm_reset);

	l3fe_reset = of_reset_control_get(np, "l3fe_reset");
	if (IS_ERR(l3fe_reset)) {
		dev_warn(&pdev->dev, "OF get l3fe_reset failed!!");
	}
	else {
		reset_control_assert(l3fe_reset);
		udelay(1);
		reset_control_deassert(l3fe_reset);
	}
	reset_control_put(l3fe_reset);

	tqm_reset = of_reset_control_get(np, "tqm_reset");
	if (IS_ERR(tqm_reset)) {
		dev_warn(&pdev->dev, "OF get tqm_reset failed!!");
	}
	else {
		reset_control_assert(tqm_reset);
		udelay(1);
		reset_control_deassert(tqm_reset);
	}
	reset_control_put(tqm_reset);

#if defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
	dma_reset = of_reset_control_get(np, "dma_reset");
	if (IS_ERR(dma_reset)) {
		dev_warn(&pdev->dev, "OF get dma_reset failed!!");
	}
	else {
		reset_control_assert(dma_reset);
		udelay(1);
		reset_control_deassert(dma_reset);
	}
	reset_control_put(dma_reset);

	printf("%s: Do external RESET for RTL8367SB HSGMII mode....\n", __func__);
	ext_reset = of_reset_control_get(np, "ext_reset");
	if (IS_ERR(ext_reset)) {
		dev_warn(&pdev->dev, "OF get ext_reset failed!!");
	}
	else {
		reset_control_assert(ext_reset);
		mdelay(200);
		reset_control_deassert(ext_reset);
	}
	reset_control_put(ext_reset);
#endif

#if defined(CONFIG_ARCH_CORTINA_G3HGU)
#if 0
	saturn_np = of_find_node_by_name(of_root, "saturn_ethernet");
	if (saturn_np == NULL) {
		printf("Fail to get device node saturn_ethernet!\n");
	} else {
		saturn_reset = of_reset_control_get(saturn_np, "saturn_ni_reset");
		if (IS_ERR(saturn_reset)) {
			dev_warn(&pdev->dev, "OF get saturn_ni_reset failed!!");
		} else {
			reset_control_assert(saturn_reset);
			mdelay(200);
			reset_control_deassert(saturn_reset);
		}

		saturn_reset = of_reset_control_get(saturn_np, "saturn_l2fe_reset");
		if (IS_ERR(saturn_reset)) {
			dev_warn(&pdev->dev, "OF get saturn_l2fe_reset failed!!");
		} else {
			reset_control_assert(saturn_reset);
			mdelay(200);
			reset_control_deassert(saturn_reset);
		}

		saturn_reset = of_reset_control_get(saturn_np, "saturn_l2tm_reset");
		if (IS_ERR(saturn_reset)) {
			dev_warn(&pdev->dev, "OF get saturn_l2tm_reset failed!!");
		} else {
			reset_control_assert(saturn_reset);
			mdelay(200);
			reset_control_deassert(saturn_reset);
		}

		saturn_reset = of_reset_control_get(saturn_np, "saturn_l3fe_reset");
		if (IS_ERR(saturn_reset)) {
			dev_warn(&pdev->dev, "OF get saturn_l3fe_reset failed!!");
		} else {
			reset_control_assert(saturn_reset);
			mdelay(200);
			reset_control_deassert(saturn_reset);
		}

		saturn_reset = of_reset_control_get(saturn_np, "saturn_sdram_reset");
		if (IS_ERR(saturn_reset)) {
			dev_warn(&pdev->dev, "OF get saturn_sdram_reset failed!!");
		} else {
			reset_control_assert(saturn_reset);
			mdelay(200);
			reset_control_deassert(saturn_reset);
		}

		saturn_reset = of_reset_control_get(saturn_np, "saturn_tqm_reset");
		if (IS_ERR(saturn_reset)) {
			dev_warn(&pdev->dev, "OF get saturn_tqm_reset failed!!");
		} else {
			reset_control_assert(saturn_reset);
			mdelay(200);
			reset_control_deassert(saturn_reset);
		}
	}
#endif // if 0
#endif

	/* Reset HSGMII */
#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	aal_scfg_read(CFG_ID_PHY_INTERFACE, CFG_ID_PHY_INTERFACE_LEN, &(phy_intfs[0]));
	if (phy_intfs[AAL_NI_PORT_GE4] != AAL_NI_IF_MODE_GE_RGMII) {
		printf("%s: Port 4 connect to HSGMII\n", __func__);
	}
	else {
		printf("%s: Port 4 connect to RGMII\n", __func__);
	}
	if (phy_intfs[AAL_NI_PORT_GE4] != AAL_NI_IF_MODE_GE_RGMII) {
		sgmii_phy = devm_phy_get(&pdev->dev, "sgmii-phy");
		if (IS_ERR(sgmii_phy)) {
			dev_warn(&pdev->dev, "OF get sgmii-phy failed!!");
		}
		else {
			printf("%s: sgmii_phy=%p\n", __func__, sgmii_phy);
			phy_power_on(sgmii_phy);
			mdelay(1);
		}

		phy_sdsif_reset = of_reset_control_get(np, "phy_sdsif_reset");
		if (IS_ERR(phy_sdsif_reset)) {
			dev_warn(&pdev->dev, "OF get phy_sdsif_reset failed!!");
		}
		else {
			reset_control_assert(phy_sdsif_reset);
			udelay(1);
			reset_control_deassert(phy_sdsif_reset);
		}
		reset_control_put(phy_sdsif_reset);

		phy_sgmii_reset = of_reset_control_get(np, "phy_sgmii_reset");
		if (IS_ERR(phy_sgmii_reset)) {
			dev_warn(&pdev->dev, "OF get phy_sgmii_reset failed!!");
		}
		else {
			reset_control_assert(phy_sgmii_reset);
			udelay(1);
			reset_control_deassert(phy_sgmii_reset);
		}
		reset_control_put(phy_sgmii_reset);

		printf("%s: Do external RESET for RTL8367SB HSGMII mode....\n", __func__);
		ext_reset = of_reset_control_get(np, "ext_reset");
		if (IS_ERR(ext_reset)) {
			dev_warn(&pdev->dev, "OF get ext_reset failed!!");
		}
		else {
			reset_control_assert(ext_reset);
			mdelay(200);
			reset_control_deassert(ext_reset);
		}
		reset_control_put(ext_reset);
	}
#endif
}
#endif
#endif//sd1 uboot for 98f - TBD

static ca_g3_init_config_t ca_ni_init_config;

static ca_status_t ca_load_init(ca_g3_init_config_t *p_cfg, ca_uint32_t size)
{
    ca_uint32_t     temp_data;

    /* load init config and call ca_init() */
    memset(p_cfg, 0, sizeof(ca_g3_init_config_t));

    /* get number of interfaces from startup config */
#if 0//yocto
    aal_scfg_read(CFG_ID_NE_NUM_INTF, 4, &ca_ni_num_intf);
#else//sd1 uboot for 98f
    ca_ni_num_intf = 2;
#endif

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("%s: ca_ni_num_intf=%d\n", __func__, ca_ni_num_intf);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    /* number of interfaces can not exceed ca_ni_a53_cpu_port_count */
    if (ca_ni_num_intf > ca_ni_a53_cpu_port_count) {
#if CONFIG_98F_UBOOT_NE_DBG
    	ca_printf("%s: ca_ni_num_intf can not bigger than ca_ni_a53_cpu_port_count!!\n", __func__);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    	ca_ni_num_intf = ca_ni_a53_cpu_port_count;
#if CONFIG_98F_UBOOT_NE_DBG
    	ca_printf("%s: adjust ca_ni_num_intf to %d\n", __func__, ca_ni_num_intf);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    }

    /* init wan_port_id */
#if 0//yocto
    aal_scfg_read(CFG_ID_WAN_PORT_ID, 1, &wan_port_id);
#else//sd8 luna for 98f
    wan_port_id = 0x03;
#endif

    p_cfg->ca_ne_port_id = wan_port_id;

#if 0//yocto
    aal_scfg_read(CFG_ID_NE_MAC_ADDR0, 6, &(p_cfg->ca_ne_mac_addr0[0]));
    aal_scfg_read(CFG_ID_NE_MAC_ADDR1, 6, &(p_cfg->ca_ne_mac_addr1[0]));

    aal_scfg_read(CFG_ID_NE_SRAM_PHY_BASE, 4, &temp_data);
#else//sd1 uboot for 98f
    p_cfg->ca_ne_mac_addr0[0] = 00;
    p_cfg->ca_ne_mac_addr0[1] = 13;
    p_cfg->ca_ne_mac_addr0[2] = 25;
    p_cfg->ca_ne_mac_addr0[3] = 00;
    p_cfg->ca_ne_mac_addr0[4] = 00;
    p_cfg->ca_ne_mac_addr0[5] = 01;

    p_cfg->ca_ne_mac_addr1[0]= 00;
    p_cfg->ca_ne_mac_addr1[1]= 13;
    p_cfg->ca_ne_mac_addr1[2]= 25;
    p_cfg->ca_ne_mac_addr1[3]= 00;
    p_cfg->ca_ne_mac_addr1[4]= 00;
    p_cfg->ca_ne_mac_addr1[5]= 02;

    temp_data = 0xc0000000;
#endif
    p_cfg->ca_ne_sram_phy_base = temp_data;

#if 0//yocto
    aal_scfg_read(CFG_ID_NE_DDR_COHERENT_PHY_BASE, 4, &temp_data);
#else//sd1 uboot for 98f
        temp_data = 0x07200000;
#endif
    p_cfg->ca_ne_ddr_coherent_phy_base = temp_data;

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
    aal_scfg_read(CFG_ID_NE_DDR_NONCACHE_PHY_BASE, 4, &temp_data);
    p_cfg->ca_ne_ddr_noncache_phy_base = temp_data;
#endif

#if 1//value from sd8 luna
    ca_ne_sram_base = 0xc8e00000;
    ca_ne_ddr_noncache_base = 0x00000000;
    ca_ne_sram_phy_base = 0xc0000000;
#endif//value from sd8 luna

    p_cfg->ca_ne_sram_base = ca_ne_sram_base;
#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d)\t p_cfg->ca_ne_sram_base: 0x%08x\n", __FUNCTION__, __LINE__, p_cfg->ca_ne_sram_base);//yocto: 0xc8e00000
    ca_printf("\t%s(%d)\t ca_ne_sram_base: 0x%08x\n", __FUNCTION__, __LINE__, ca_ne_sram_base);//yocto: 0xc8e00000
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    p_cfg->ca_ne_ddr_noncache_base = ca_ne_ddr_noncache_base;
#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d)\t p_cfg->ca_ne_ddr_noncache_base: 0x%08x\n", __FUNCTION__, __LINE__, p_cfg->ca_ne_ddr_noncache_base);//yocto: 0x00000000
    ca_printf("\t%s(%d)\t ca_ne_ddr_noncache_base: 0x%08x\n", __FUNCTION__, __LINE__, ca_ne_ddr_noncache_base);//yocto: 0x00000000
#endif /* CONFIG_98F_UBOOT_NE_DBG */

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d)\t p_cfg->ca_ne_sram_phy_base: 0x%08x\n", __FUNCTION__, __LINE__, p_cfg->ca_ne_sram_phy_base);//yocto: 0xc0000000
    ca_printf("\t%s(%d)\t ca_ne_sram_phy_base: 0x%08x\n", __FUNCTION__, __LINE__, ca_ne_sram_phy_base);//yocto: 0xc0000000
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    /* check consistent between device tree and startup config */
    if (p_cfg->ca_ne_sram_phy_base != ca_ne_sram_phy_base) {
#if CONFIG_98F_UBOOT_NE_DBG
    	ca_printf("%s: please make sure CFG_ID_NE_SRAM_PHY_BASE=0x%x consistent with device tree!!\n", __func__, p_cfg->ca_ne_sram_phy_base);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    }

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d)\tp_cfg->ca_ne_ddr_noncache_phy_base: 0x%08x\n", __FUNCTION__, __LINE__, p_cfg->ca_ne_ddr_noncache_phy_base);//yocto: 0x0
    ca_printf("\t%s(%d)\t(ca_ne_ddr_noncache_phy_base & 0xffffffff): 0x%08x\n", __FUNCTION__, __LINE__, (ca_ne_ddr_noncache_phy_base & 0xffffffff));//yocto: 0x0
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    if (p_cfg->ca_ne_ddr_noncache_phy_base != (ca_ne_ddr_noncache_phy_base & 0xffffffff)) {
#if CONFIG_98F_UBOOT_NE_DBG
    	ca_printf("%s: please make sure CFG_ID_NE_DDR_NONCACHE_PHY_BASE=0x%x consistent with device tree!!\n", __func__, p_cfg->ca_ne_ddr_noncache_phy_base);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    }
#endif

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("\t%s(%d)\t p_cfg->ca_ne_ddr_coherent_phy_base: 0x%08x\n", __FUNCTION__, __LINE__, p_cfg->ca_ne_ddr_coherent_phy_base);//yocto: 0x07200000
    ca_printf("\t%s(%d)\t ca_ne_ddr_coherent_phy_base: 0x%08x\n", __FUNCTION__, __LINE__, ca_ne_ddr_coherent_phy_base);//yocto: 0x07200000
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if (p_cfg->ca_ne_ddr_coherent_phy_base != ca_ne_ddr_coherent_phy_base) {
#if CONFIG_98F_UBOOT_NE_DBG
    	ca_printf("%s: please make sure CFG_ID_NE_DDR_COHERENT_PHY_BASE=0x%x consistent with device tree!!\n", __func__, p_cfg->ca_ne_ddr_coherent_phy_base);
#endif /* CONFIG_98F_UBOOT_NE_DBG */
    }
    memcpy(&(port_cfg_infos[0].mac_addr[0]), &(p_cfg->ca_ne_mac_addr0[0]), 6);
    memcpy(&(port_cfg_infos[1].mac_addr[0]), &(p_cfg->ca_ne_mac_addr1[0]), 6);

#if CONFIG_98F_UBOOT_NE_DBG
    ca_printf("=================ca_ni_init_config================\n");
    ca_printf("%s: ca_ne_sram_base=%p, ca_ne_ddr_noncache_base=%p\n", __func__, p_cfg->ca_ne_sram_base, p_cfg->ca_ne_ddr_noncache_base);
    ca_printf("%s: ca_ne_sram_phy_base=0x%x, ca_ne_ddr_noncache_phy_base=0x%x, ca_ne_ddr_coherent_phy_base=0x%x\n",
    	__func__, p_cfg->ca_ne_sram_phy_base, p_cfg->ca_ne_ddr_noncache_phy_base, p_cfg->ca_ne_ddr_coherent_phy_base);
    ca_printf("%s: ca_ne_mac_addr0=%pM, ca_ne_mac_addr1=%pM\n", __func__, &(p_cfg->ca_ne_mac_addr0[0]), &(p_cfg->ca_ne_mac_addr1[0]));
    ca_printf("==================================================\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    return CA_E_OK;
}

static void ca_ni_a17_rst_pe0(bool rst_pe0){
    GLOBAL_BLOCK_RESET_SECURE_t global_block_reset_secure;
    global_block_reset_secure.wrd = CA_REG_READ(GLOBAL_BLOCK_RESET_SECURE);

    /* 1'b1: assert pe0 reset, halt pe0 */
    /* 1'b0: de-assert pe0 reset, let pe0 free run */
    global_block_reset_secure.bf.reset_pe0 = rst_pe0;

    CA_REG_WRITE(global_block_reset_secure.wrd, GLOBAL_BLOCK_RESET_SECURE);
}

static void ca_ni_a17_cfg_pe0_sram_mode(void){
    PE_CTRL_PE_SRAM_CONFIG_t pe_ctrl_pe_sram_config;
//    pe_ctrl_pe_sram_config.wrd = CA_REG_READ(PE_CTRL_PE_SRAM_CONFIG);

    pe_ctrl_pe_sram_config.wrd = 0x14;

    CA_REG_WRITE(pe_ctrl_pe_sram_config.wrd, PE_CTRL_PE_SRAM_CONFIG);
}

static void ca_ni_a17_cfg_sram_secure(void){
    CAPSRAM_TZCONTROL_t capsram_tzcontrol;
//    capsram_tzcontrol.wrd = CA_REG_READ(CAPSRAM_TZCONTROL);

    /* 1'b1: secure */
    /* 1'b0: NS */
    capsram_tzcontrol.wrd = 0x0;

    CA_REG_WRITE(capsram_tzcontrol.wrd, CAPSRAM_TZCONTROL);
}

int ca_ni_init_module_probe(void)
{
    int i, ret;
    int budget;
    //ca_init_config_t init_config;
    ca_init_err_t err_code;

#if 0//sd1 uboot for 98f
    ca_eth_private_t *cep = platform_get_drvdata(pdev);
    //struct aal_fbm_pool_config_s  fbm_pool;
#ifdef CONFIG_OF
    struct device_node *np = pdev->dev.of_node;
#endif

    pr_notice("%s %s\n", DRV_NAME, DRV_VERSION);
#endif//sd1 uboot for 98f

    if (ni_initialized){
#if 0//yocto
        printf("%s(%d) %s\tThe system has been initialized\n", __func__, __LINE__, __FILE__);
#else//sd1 uboot for 98f
#if CONFIG_98F_UBOOT_NE_DBG
        printf("\t%s(%d) %s\tThe NE has been initialized before!\n", __func__, __LINE__, __FILE__);
#else
        printf("The NE has been initialized before!\n");
#endif /* CONFIG_98F_UBOOT_NE_DBG */
#endif//sd1 uboot for 98f
        return 0;
    }

    /* halt pe0 rst for dma_lso to qm loopback test */
    ca_ni_a17_rst_pe0(1);

    /* config SRAM to be NS for MP PE test */
    ca_ni_a17_cfg_pe0_sram_mode();
    ca_ni_a17_cfg_sram_secure();

#if 0//sd1 uboot for 98f - TBD
    if ((ret = ca_ne_reg_init(pdev)) != 0) {
    	printf("%s: Fail to do ca_ne_reg_init(). (ret=%d)\n", __func__, ret);

#if 0//yocto
    	return -ENXIO;
#else//sd1 uboot
    	return CA_E_ERROR;
#endif
    }
#endif//sd1 uboot for 98f - TBD

#if 0//sd1 uboot for 98f - TBD
    if (!SOC_HAS_NI()) {
    	printf("%s: No NI hardware module, fail to load driver.\n", __func__);
#if 0//yocto
    	return -ENODEV;
#else//sd1 uboot
    	return CA_E_ERROR;
#endif
    }

    ca_mut_init();

#ifdef CONFIG_OF
    ca_ni_global_reset(pdev);
#endif

    memset(&ni_info_data, 0, sizeof(ni_info_data));

    /* record pdev for later use */
    ni_info_data.pdev = pdev;
#endif//sd1 uboot for 98f - TBD

    /* load init config and call ca_init() */
    ca_load_init(&ca_ni_init_config, sizeof(ca_g3_init_config_t));

#if 1//value from sd8 luna
    ca_ni_init_config.ca_ni_mgmt_mode = 0x0;
    ca_ni_init_config.ca_ne_port_id = 0x3;
#endif

    ca_init(0, (ca_init_config_t *)&ca_ni_init_config, &err_code);
#if CONFIG_98F_UBOOT_NE_DBG
    printf("%s: ca_init(), error code=%d\n", __func__, err_code);
#endif /* CONFIG_98F_UBOOT_NE_DBG */

    if(CA_E_OK == err_code){
        ni_initialized = 1;

        /* halt pe0 rst for dma_lso to qm loopback test */
        ca_ni_a17_rst_pe0(0);

        ret = CA_E_OK;
    }else{
        ret = CA_E_ERROR;
    }

#if 0//sd1 uboot for 98f - TBD

    aal_l3qm_eq_get_a53_pool0_1_start_addr(&ca_ne_a53_pool0_start_addr, &ca_ne_a53_pool1_start_addr);

    /* enable refill threshold interrupts for CPU 64 */
    aal_l3qm_setup_refill_threshold_interrup();

    ca_ni_init_cpu_ports(pdev);

    /// init a reset task
    INIT_WORK(&ni_info_data.reset_task, ca_ni_reset_task);


    /* should after ca_ni_init_cpu_ports(), to wait the net_device be created */
    /* initialize CPU EPP FIFO and Empty Buffer Pools */
    if (!ca_ni_mgmt_mode) {
    	/* move to ca_init() */
    	//ca_ni_init_l3qm();
    	//ca_ni_init_fbm();

    	/* if EQ pool use FBM no need to fill EQ buffer pool */
    	if (aal_l3qm_eq_get_a53_use_fbm() == 0) {
    		/* only need to call ca_ni_fill_eq_buf_pool() once to fill both A53 EQs */
    		printf("%s: ======= before A53 pool bid count=%d =======\n", __func__, aal_l3qm_eq_get_a53_pool_bid_count());
    		ca_ni_fill_eq_buf_pool(ni_info_data.dev[0], 0, aal_l3qm_eq_get_a53_pool_bid_count());
    		wmb();
    		printf("%s: ======= after A53 pool bid count=%d =======\n", __func__, aal_l3qm_eq_get_a53_pool_bid_count());
    	}

    	ni_info_data.refill_req_0 = irq_of_parse_and_map(np, CA_NI_IRQ_QM_CPU_REFILL_REQ_8);        /* QM EQ refill threshold interrupt */
    	ni_info_data.refill_req_1 = irq_of_parse_and_map(np, CA_NI_IRQ_QM_CPU_REFILL_REQ_9);        /* QM EQ refill threshold interrupt */
    	printf("%s: ni_info_data.refill_req_0=%d, ni_info_data.refill_req_1=%d\n", __func__,
    		ni_info_data.refill_req_0, ni_info_data.refill_req_1);

    	ca_ni_init_tx_dma_lso(pdev);
    }

#ifdef NI_RECYCLE_SKB
    skb_queue_head_init(&ca_ni_skb_recycle_cpu0_head);
#endif

    /* read budget from startup config */
    if (aal_scfg_read(CFG_ID_NE_NAPI_BUDGET, 4, &budget) == CA_E_OK) {
    	/* the budget == 0 is not allowed */
        	if (budget > 0) {
        		ni_napi_budget = budget;
        	}
        }
        printf("%s: ni_napi_budget=%d\n", __func__, ni_napi_budget);

        /* management mode only support one port from/to CPU */
        if (ca_ni_mgmt_mode) {

        /* add NAPI for cpu ports */
        cep = netdev_priv(ni_info_data.dev[CPU_PORT0_INST]);
        netif_napi_add(ni_info_data.dev[CPU_PORT0_INST], &cep->napi, ca_ni_poll_0, ni_napi_budget);

        //cep->port_cfg.port_id[0] = CA_NI_PORT5;
        cep->mgmt_priv.rx_xram_base_addr  = (ca_uint_t)AAL_XRAM_REG_ADDR(CA_NI_RX_BASE_ADDR * 8);
        cep->mgmt_priv.rx_xram_end_addr   = (ca_uint_t)AAL_XRAM_REG_ADDR((CA_NI_RX_TOP_ADDR + 1) * 8);
        cep->mgmt_priv.rx_xram_start      = CA_NI_RX_BASE_ADDR;
        cep->mgmt_priv.rx_xram_end        = CA_NI_RX_TOP_ADDR;
        cep->mgmt_priv.tx_xram_base_addr  = (ca_uint_t)AAL_XRAM_REG_ADDR(CA_NI_TX_BASE_ADDR * 8);
        cep->mgmt_priv.tx_xram_end_addr   = (ca_uint_t)AAL_XRAM_REG_ADDR((CA_NI_TX_TOP_ADDR + 1) * 8);
        cep->mgmt_priv.tx_xram_start      = CA_NI_TX_BASE_ADDR;
        cep->mgmt_priv.tx_xram_end        = CA_NI_TX_TOP_ADDR;

        printf("%s: cep->mgmt_priv.rx_xram_base_addr=0x%lx, cep->mgmt_priv.rx_xram_end_addr=0x%lx\n", __func__,
            (unsigned long)cep->mgmt_priv.rx_xram_base_addr, (unsigned long)cep->mgmt_priv.rx_xram_end_addr);
        printf("%s: cep->mgmt_priv.tx_xram_base_addr=0x%lx, cep->mgmt_priv.tx_xram_end_addr=0x%lx\n", __func__,
            (unsigned long)cep->mgmt_priv.tx_xram_base_addr, (unsigned long)cep->mgmt_priv.tx_xram_end_addr);
    }
    else {
        /* add NAPI for cpu ports */
        for ( i = 0; i < ca_ni_num_intf; i++) {
            cep = netdev_priv(ni_info_data.dev[i]);
            switch (i) {
                case 0:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_0, ni_napi_budget);
                    break;
                case 1:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_1, ni_napi_budget);
                    break;
                case 2:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_2, ni_napi_budget);
                    break;
                case 3:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_3, ni_napi_budget);
                    break;
                case 4:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_4, ni_napi_budget);
                    break;
                case 5:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_5, ni_napi_budget);
                    break;
                case 6:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_6, ni_napi_budget);
                    break;
                case 7:
                    netif_napi_add(ni_info_data.dev[i], &cep->napi, ca_ni_poll_7, ni_napi_budget);
                    break;
            }
        }
    }

#ifdef CONFIG_PROC_FS
    ca_ni_proc_init();
    ca_aal_proc_init();
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU
    ca_api_proc_init();
#endif
#endif

    /* create NETLINK */
    ca_ni_netlink_igmpsnooping_init();
    ca_ni_netlink_rx_init();

    /* register_netdev() should be the last one */
    ca_ni_register_netdev();

    spin_lock_init(&ca_ni_rx_refill_lock);

#ifdef CONFIG_CA_NE_L2FP
    ca_l2fp_init();
#endif

#endif//sd1 uboot for 98f - TBD

    return ret;
}

#if 0//sd1 uboot for 98f - TBD
static int ca_ni_cleanup_module_remove(struct platform_device *pdev)
{
	int i;
	ni_info_t *ni_info;
	ca_eth_private_t *cep = platform_get_drvdata(pdev);

	ni_info = &ni_info_data;

	/* free the memory allocated for DMA LSO */
	ca_ni_cleanup_tx_dma_lso(pdev);

	for (i = 0; i < ca_ni_num_intf; i++) {

#ifdef CONFIG_CA_NE_VIRTUAL_NI
		ca_ni_virt_ni_remove_if(i);
#endif
		cep = netdev_priv(ni_info_data.dev[i]);
		unregister_netdev(cep->dev);
		free_netdev(ni_info_data.dev[i]);
	}

#ifdef CONFIG_PROC_FS
	ca_ni_proc_exit();
	ca_aal_proc_exit();
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_api_proc_exit();
#endif
#endif

	/* unregister NETLINK */
	ca_ni_netlink_igmpsnooping_exit();
	ca_ni_netlink_rx_exit();

#ifdef CONFIG_CA_NE_L2FP
	ca_l2fp_cleanup();
#endif

	return 0;
}

#ifdef CONFIG_PM
static int ca_ni_resume(struct platform_device *pdev)
{

	/// for each GMAC port
	///   get net_device dev
	///   if (netif_running(dev)) {
	///     netif_device_attach(dev);

	///     These depends on how far the NI suspends
	///     phy_start()
	///     of_phy_connect() ???
	///     enable NAPI

	///     enable NI interrupt

	return 0;
}

static int ca_ni_suspend(struct platform_device *pdev, pm_message_t state)
{

	/// for each GMAC port
	///   get net_device dev
	///   if (netif_running(dev)) {
	///     netif_device_detach(dev);
	///     netif_stop_queue(dev);

	///     stop NI function, or maybe this act should be earlier than kernel handler?
	///     free RX buffer, stop TX timer

	///     CS7884 needs to handle PHY? it depends on how far the NI suspends
	///     phy_stop()
	///     phy_disconnect()

	///     stop NI interrupt

	return 0;
}
#else /* CONFIG_PM */
#define ca_ni_resume	NULL
#define ca_ni_suspend	NULL
#endif /* CONFIG_PM */

#include <linux/init.h>

/* ca_ne_reg_xxx() should move to cs_init() */
static void __iomem *ca_ne_reg_base = NULL;
static void __iomem *ca_ne_glb_ne_intr_reg_base = NULL;
//static void __iomem *ca_glb_reg_base = NULL;
static void __iomem *ca_xram_reg_base = NULL;
static void __iomem *ca_dma_lso_reg_base = NULL;
static void __iomem *ca_per_mdio_reg_base = NULL;
//void __iomem *ca_ne_sram_base = NULL;
//void __iomem *ca_ne_ddr_reserved_base = NULL;


#ifndef CONFIG_CORTINA_BOARD_FPGA
static void __iomem *ca_ne_phy_reg_base = NULL;
#endif

#if defined(CONFIG_CA_NE_AAL_XFI)
static void __iomem *ca_ne_xfi_reg_base = NULL;
#endif

#define CA_NI_BASE_ADDR (L2FE_GLB_VER_CTRL & 0xFFFF0000)
#define CA_GLB_BASE_ADDR GLOBAL_JTAG_ID
#define CA_XRAM_BASE_ADDR CA_NI_XRAM_BASE

#ifdef CONFIG_OF
#define CA_NI_MEM_RESOURCE_NI_REGS      0
#define CA_NI_MEM_RESOURCE_GLOBAL_NETWORK_ENGINE_INTERRUPT_REGS  1
#define CA_NI_MEM_RESOURCE_MDIO_REGS    2
#define CA_NI_MEM_RESOURCE_XRAM         3
#define CA_NI_MEM_RESOURCE_DMA_LSO      4
#define CA_NI_MEM_RESOURCE_SRAM         5
#define CA_NI_MEM_RESOURCE_DDR_COHERENT          6
#define CA_NI_MEM_RESOURCE_DDR_NONCACHE          7

#ifndef CONFIG_CORTINA_BOARD_FPGA
#define CA_NI_MEM_RESOURCE_PHY_REGS     8
#endif

#if defined(CONFIG_CA_NE_AAL_XFI)
#define CA_NI_MEM_RESOURCE_XFI_REGS     9
#endif

static const struct of_device_id cortina_ni_of_match[] __initconst;
#endif

extern ca_uint32_t aal_hgu_8279_ne_reg_base_init(void **p_iobase);
#endif//sd1 uboot - TBD

#if 0//sd1 uboot for 98f - TBD
static int ca_ne_reg_init(struct platform_device *pdev)
{
	int ret;
	ca_uint_t ddr_coherent_base_addr;

	/* get the resource map from device tree for NI/L2FE/L3FE/TQM module registers */
#if CONFIG_OF /* DT support */
	/* assign DT node pointer */
	struct device_node *np = pdev->dev.of_node;
	struct resource mem_resource;
	const struct of_device_id *match;

	/* search DT for a match */
	match = of_match_device(cortina_ni_of_match, &pdev->dev);
	if (!match)
		return -EINVAL;

	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_NI_REGS, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_NI_REGS) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: mem_resource for NI registers=%pR\n", __func__, &mem_resource);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_ne_reg_base = mem_resource.start;
#else
	ca_ne_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_ne_reg_base) {
		printf("%s: ioremap(CA_NI_BASE_ADDR=0x%x) 1M failed!!!\n", __func__, CA_NI_BASE_ADDR);
		return -ENOMEM;
	}
#endif

	printf("%s: ca_ne_reg_base=%p\n", __func__, ca_ne_reg_base);

	/* search DT for a match */
	match = of_match_device(cortina_ni_of_match, &pdev->dev);
	if (!match)
		return -EINVAL;

	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_GLOBAL_NETWORK_ENGINE_INTERRUPT_REGS, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_GLOBAL_NETWORK_ENGINE_INTERRUPT_REGS) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: mem_resource for GLOBAL NETWORK ENGINE INTRRUPT registers=%pR\n", __func__, &mem_resource);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_ne_glb_ne_intr_reg_base = mem_resource.start;
#else
	ca_ne_glb_ne_intr_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_ne_glb_ne_intr_reg_base) {
		printf("%s: GLOBAL NETWORK ENGINE INTRRUPT registers ioremap failed!!!\n", __func__);
		return -ENOMEM;
	}
#endif

	printf("%s: ca_ne_glb_ne_intr_reg_base=%p\n", __func__, ca_ne_glb_ne_intr_reg_base);

	aal_ne_reg_base_init(ca_ne_reg_base);
	aal_glb_ne_intr_reg_base_init(ca_ne_glb_ne_intr_reg_base);
#if defined(CONFIG_ARCH_CORTINA_G3HGU)
	aal_hgu_8279_ne_reg_base_init(&hgu_8279_ne_reg_iobase);
	printf("%s: hgu_8279_ne_reg_iobase=%p\n",__func__,hgu_8279_ne_reg_iobase);
#endif

	/* get the resource map from device tree for PER_MDIO module registers */
	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_MDIO_REGS, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_MDIO_REGS) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: mem_resource for MDIO registers=%pR\n", __func__, &mem_resource);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_per_mdio_reg_base = mem_resource.start;
#else
	ca_per_mdio_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_per_mdio_reg_base) {
		printf("%s: MDIO registers ioremap() failed!!!\n", __func__);
		return -ENOMEM;
	}
#endif
	printf("%s: ca_per_mdio_reg_base=%p\n", __func__, ca_per_mdio_reg_base);
	aal_per_mdio_reg_base_init(ca_per_mdio_reg_base);

	/* get the resource map from device tree for XRAM */
	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_XRAM, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_XRAM) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: resource for XRAM=%pR\n", __func__, &mem_resource);
	ca_xram_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_xram_reg_base) {
		printf("%s: ioremap(CA_XRAM_BASE_ADDR=0x%x) SZ_8K failed!!!\n", __func__, CA_XRAM_BASE_ADDR);
		return -ENOMEM;
	}
	printf("%s: ca_xram_reg_base=%p\n", __func__, ca_xram_reg_base);
	aal_xram_reg_base_init(ca_xram_reg_base);

	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_DMA_LSO, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_DMA_LSO) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: mem_resource for DMA LSO registers=%pR\n", __func__, &mem_resource);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_dma_lso_reg_base = mem_resource.start;
#else
	ca_dma_lso_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_dma_lso_reg_base) {
		printf("%s: DMA LSO ioremap() failed!!!\n", __func__);
		return -ENOMEM;
	}
#endif
	printf("%s: ca_dma_lso_reg_base=%p\n", __func__, ca_dma_lso_reg_base);
	aal_dma_reg_base_init(ca_dma_lso_reg_base);


	/* get the resource map from device tree for SRAM */
	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_SRAM, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_SRAM) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: resource for SRAM=%pR\n", __func__, &mem_resource);
	ca_ne_sram_phy_base = mem_resource.start;
	ca_ne_sram_size = resource_size(&mem_resource);
	if (mem_resource.end > (CA_NI_SRAM_BASE_ADDR + CA_NI_SRAM_SIZE)) {
		printf("%s: out of SRAM range %p NE driver initialized failed!!!\n", __func__, mem_resource.end);
		BUG_ON(0);
	}
	printf("%s: ca_ne_sram_phy_base=0x%llx end %p size 0x%x\n", __func__, (ca_uint64_t)ca_ne_sram_phy_base, mem_resource.end, resource_size(&mem_resource));
#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_ne_sram_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_ne_sram_base) {
		printf("%s: SRAM memory ioremap() failed!!!\n", __func__);
		return -ENOMEM;
	}
#else
	ca_ne_sram_base = CA_KSEG1_ATU(mem_resource.start);
#endif
	printf("%s: ca_ne_sram_base=0x%lx\n", __func__, (unsigned long)ca_ne_sram_base);

	/* get the resource map from device tree for DDR coherent */
	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_DDR_COHERENT, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_DDR_COHERENT) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: resource for DDR=%pR\n", __func__, &mem_resource);

	ddr_coherent_base_addr = mem_resource.start;
	ca_ne_ddr_coherent_base = (void __iomem *)ddr_coherent_base_addr;
	ca_ne_ddr_coherent_phy_base = ddr_coherent_base_addr;
	printf("%s: ca_ne_ddr_coherent_base=%p\n", __func__, ca_ne_ddr_coherent_base);

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	/* get the resource map from device tree for DDR non-cache  */
	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_DDR_NONCACHE, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_DDR_NONCACHE) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: resource for DDR non-cache=%pR\n", __func__, &mem_resource);

	//ddr_noncache_base_addr = mem_resource.start;
	ca_ne_ddr_noncache_phy_base = mem_resource.start;

	ca_ne_ddr_noncache_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_ne_ddr_noncache_base) {
		printf("%s: DDR noncache memory ioremap() failed!!!\n", __func__);
		return -ENOMEM;
	}
	printf("%s: ca_ne_ddr_noncache_base=%p, ca_ne_ddr_noncache_phy_base=0x%llx\n", __func__,
		ca_ne_ddr_noncache_base, (ca_uint64_t)ca_ne_ddr_noncache_phy_base);
#endif


#ifndef CONFIG_CORTINA_BOARD_FPGA

	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_PHY_REGS, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_PHY_REGS) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: mem_resource for ASIC GPHY registers=%pR\n", __func__, &mem_resource);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_ne_phy_reg_base = mem_resource.start;
#else
	ca_ne_phy_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_ne_phy_reg_base) {
		printf("%s: ioremap(CA_NI_MEM_RESOURCE_PHY_REGS=0x%x) failed!!!\n", __func__, mem_resource.start);
		return -ENOMEM;
	}
#endif

	printf("%s: ca_ne_phy_reg_base=%p\n", __func__, ca_ne_phy_reg_base);

	aal_ne_phy_reg_base_init(ca_ne_phy_reg_base);

#if defined(CONFIG_CA_NE_AAL_XFI)
	/* get "reg" property from DT and convert to platform mem address resource */
	ret = of_address_to_resource(np, CA_NI_MEM_RESOURCE_XFI_REGS, &mem_resource);
	if (ret) {
		printf("%s: of_address_to_resource(CA_NI_MEM_RESOURCE_XFI_REGS) return %d\n", __func__, ret);
		return ret;
	}

	printf("%s: mem_resource for XFI registers=%pR\n", __func__, &mem_resource);

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU
	ca_ne_xfi_reg_base = mem_resource.start;
#else
	ca_ne_xfi_reg_base = devm_ioremap(&pdev->dev, mem_resource.start, resource_size(&mem_resource));
	if (!ca_ne_xfi_reg_base) {
		printf("%s: ioremap(CA_NI_MEM_RESOURCE_XFI_REGS=0x%x) failed!!!\n", __func__, mem_resource.start);
		return -ENOMEM;
	}
#endif

	printf("%s: ca_ne_xfi_reg_base=%p\n", __func__, ca_ne_xfi_reg_base);

	aal_ne_xfi_reg_base_init(ca_ne_xfi_reg_base);
#endif
#endif

#endif /* end of CONFIG_OF */

	return 0;
}
#endif//sd1 uboot for 98f - TBD

#if 0//sd1 uboot for 98f - TBD
void ca_ne_reg_exit(void)
{
	iounmap(ca_ne_reg_base);
	iounmap(ca_ne_sram_base);
	iounmap(ca_ne_ddr_coherent_base);

#if defined(CONFIG_ARCH_CORTINA_G3) || defined(CONFIG_ARCH_CORTINA_G3HGU)
	iounmap(ca_ne_ddr_noncache_base);
#endif
}
#endif//sd1 uboot for 98f - TBD

#if 0//yocto
struct net_device * ca_ni_virtual_instance_alloc(int cpu_idx, char *name)
{
	struct net_device *dev;
	ca_eth_private_t *cep;
	int dev_idx = cpu_idx;
	aal_ni_hv_glb_internal_port_id_cfg_t ni_port_id_cfg;

#ifdef CONFIG_NET_SCH_MULTIQ
	dev = alloc_etherdev_mq(sizeof(ca_eth_private_t), QUEUE_PER_INSTANCE);
#else
	dev = alloc_etherdev(sizeof(ca_eth_private_t));
#endif

	if (dev == NULL)
		return NULL;

	snprintf(dev->name, IFNAMSIZ, name);
	dev->mtu = 1500;

	ni_info_data.dev[dev_idx] = dev;
	cep = netdev_priv(dev);
	cep->dev = dev;
	/* add a cpu_port for recognize */
	cep->cpu_port = cpu_idx;
	cep->port_cfg = port_cfg_infos[dev_idx];
	cep->use_default_lso_para0 = 1;
	cep->default_lso_para0 = 0;
	cep->deep_q = 1;

	aal_ni_hv_glb_internal_port_id_cfg_get(0, &ni_port_id_cfg);
	if (ni_port_id_cfg.dma_to_l3fe & (1 << dev_idx))
		cep->dma_l3fe = 1;
	else
		cep->dma_l3fe = 0;

	cep->napi.dev = dev;
	cep->pdev = ni_info_data.pdev;

	switch (cpu_idx) {
		case 0:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_0, ni_napi_budget);
			break;
		case 1:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_1, ni_napi_budget);
			break;
		case 2:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_2, ni_napi_budget);
			break;
		case 3:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_3, ni_napi_budget);
			break;
		case 4:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_4, ni_napi_budget);
			break;
		case 5:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_5, ni_napi_budget);
			break;
		case 6:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_6, ni_napi_budget);
			break;
		case 7:
			netif_napi_add(dev, &cep->napi, ca_ni_poll_7, ni_napi_budget);
			break;
	}

	/* initialize spin locks */
	spin_lock_init(&cep->stats_lock);
	spin_lock_init(&cep->lock);
	return dev;
}

ca_status_t ca_ni_virtual_instance_free(int cpu_idx)
{
	struct net_device *dev;
	ca_eth_private_t *cep;
	int dev_idx = cpu_idx;

	dev = ni_info_data.dev[dev_idx];
	if (dev) {
		ca_ni_virtual_instance_close(cpu_idx);
		free_netdev(dev);
	}
	return CA_E_OK;
}

ca_status_t ca_ni_virtual_instance_open(int cpu_idx)
{
	struct net_device *dev;
	ca_eth_private_t *cep;
	int dev_idx = cpu_idx;
	int retval;

	dev = ni_info_data.dev[dev_idx];
	if (dev) {
		cep = netdev_priv(dev);

		if (dev->flags & IFF_UP) {
			printf("%s: already up flags 0x%x\n", __func__, dev->flags);
			return CA_E_ERROR;
		}

		dev->flags |= IFF_UP;

		/* should dirable RX interrupt first to avoid fake RX interrrupt comes in */
		aal_ni_enable_rx_interrupt(cpu_idx, 0, ca_ni_mgmt_mode);

		netif_carrier_off(dev);

		/* request irq for this physical port */
		printf("%s: cep->port_cfg.irq=%d\n", __func__, cep->port_cfg.irq);

#ifndef CONFIG_ARCH_CORTINA_SATURN_SFU
		retval = request_irq(cep->port_cfg.irq, ca_ni_rx_interrupt, 0, dev->name, dev);
		if (retval != 0 && retval != -EBUSY) {
			printf("%s: Unable to request IRQ %d. (errno %d)\n", __func__, cep->port_cfg.irq, retval);
			return CA_E_ERROR;
		}
#endif

		/* should enable EPP interrupt after request_irq() done */
		aal_l3qm_enable_cpu_epp_interrupt(cep->cpu_port, 1);

		setup_timer(&cep->tx_completion_timer, ca_ni_tx_completion_timer_cb, (unsigned long)dev);

		cep->status = 1;
		cep->ni_driver_state = 1;
		cep->tx_completion_queues = 0;

#ifdef NI_RECYCLE_SKB
		ca_ni_prealloc_free_buffer(dev);
#endif


		//TODO: if need flow control
		//ca_ni_set_flow_control(&(cep->port_cfg));

		/* napi should be enabled before RX interrupt enabled */
		napi_enable(&cep->napi);

		/* Enable Rx interrupts at this point */
		aal_ni_enable_rx_interrupt(cep->cpu_port, 1, ca_ni_mgmt_mode);

		/* enable QM TX function for cpu port */
		aal_l3qm_enable_tx_cpu(cep->cpu_port, 1);

		return CA_E_OK;
	}

	return CA_E_ERROR;
}

ca_status_t ca_ni_virtual_instance_close(int cpu_idx)
{
	struct net_device *dev;
	ca_eth_private_t *cep;
	int dev_idx = cpu_idx;

	dev = ni_info_data.dev[dev_idx];
	if (dev) {
		cep = netdev_priv(dev);
		dev->flags &= ~IFF_UP;

		/* disable QM TX function for cpu port */
		aal_l3qm_enable_tx_cpu(cep->cpu_port, 0);

		aal_ni_enable_rx_interrupt(cep->cpu_port, 0, ca_ni_mgmt_mode);

		/* stop TX queues */
		netif_tx_disable(dev);

		/* disable NAPI */
		napi_disable(&cep->napi);

		/* release irq */
		free_irq(cep->port_cfg.irq, dev);

		cep->status = 0;
	}
	return CA_E_OK;
}

/*
 * if dest_port == 0xFF, then not bypass L2FE
 * else, bypass L2FE and directly send to physical port
 */
ca_status_t ca_ni_virtual_instance_xmit(int cpu_idx,
		struct sk_buff *skb, ca_ni_tx_config_t *p_tx_config)
{
	struct net_device *dev;
	ca_eth_private_t *cep;
	int dev_idx = cpu_idx;


	dev = ni_info_data.dev[dev_idx];
	if (dev) {
		cep = netdev_priv(dev);

		if (p_tx_config->core_config.bf.ldpid == 0xFF)
			p_tx_config->core_config.bf.ldpid = cep->port_cfg.tx_ldpid; /*0x18 or 0x19*/

		if (__ca_ni_start_xmit(skb, dev, p_tx_config) == NETDEV_TX_OK) {
			ca_skb_in(skb, true);
			return CA_E_OK;
		} else
			return CA_E_RESOURCE;
	}
	return CA_E_NOT_FOUND;
}

/* Match table for of_platform binding */
static const struct of_device_id cortina_ni_of_match[] __initconst = {
	{ .compatible = "cortina,ni-interface", },
	{}
};
MODULE_DEVICE_TABLE(of, cortina_ni_of_match);

static struct platform_driver ca_ni_platform_driver = {
	.probe		= ca_ni_init_module_probe,
	.remove		= ca_ni_cleanup_module_remove,
	.resume		= ca_ni_resume,
	.suspend	= ca_ni_suspend,
	.driver = {
		.name		= "ca-ni",
		.bus		= &platform_bus_type,
		.owner		= THIS_MODULE,
		.of_match_table = cortina_ni_of_match,
	},
};

static int __init ca_ni_init_module(void)
{
	int ret;

	if (ca_ni_mgmt_mode)
		ca_ni_num_intf = 1;

	ret = platform_driver_register(&ca_ni_platform_driver);

	printf("%s: platform_driver_register() return ret=%d\n", __func__, ret);

	return ret;
}

static void __exit ca_ni_cleanup_module(void)
{
	/* need put before platform_driver_unregister() */
	aal_exit();

	platform_driver_unregister(&ca_ni_platform_driver);
}

module_init(ca_ni_init_module);
module_exit(ca_ni_cleanup_module);

#else//sd1 uboot - TBD
#if 0
/*
 * cpu_bit_bitmap[] is a special, "compressed" data structure that
 * represents all NR_CPUS bits binary values of 1<<nr.
 *
 * It is used by cpumask_of() to get a constant address to a CPU
 * mask value that has a single bit set only.
 */

/* cpu_bit_bitmap[0] is empty - so we can back into it */
#define MASK_DECLARE_1(x)	[x+1][0] = (1UL << (x))
#define MASK_DECLARE_2(x)	MASK_DECLARE_1(x), MASK_DECLARE_1(x+1)
#define MASK_DECLARE_4(x)	MASK_DECLARE_2(x), MASK_DECLARE_2(x+2)
#define MASK_DECLARE_8(x)	MASK_DECLARE_4(x), MASK_DECLARE_4(x+4)

const unsigned long cpu_bit_bitmap[BITS_PER_LONG+1][BITS_TO_LONGS(NR_CPUS)] = {
	MASK_DECLARE_8(0),	MASK_DECLARE_8(8),
	MASK_DECLARE_8(16),	MASK_DECLARE_8(24),
#if BITS_PER_LONG > 32
	MASK_DECLARE_8(32),	MASK_DECLARE_8(40),
	MASK_DECLARE_8(48),	MASK_DECLARE_8(56),
#endif
};
EXPORT_SYMBOL_GPL(cpu_bit_bitmap);

#endif//sd1 uboot - TBD

#endif

