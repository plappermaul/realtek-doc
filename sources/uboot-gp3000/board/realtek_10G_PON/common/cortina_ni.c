/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Access Incorporated. Any use or disclosure,  */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Access Incorporated is     */
/* strictly prohibited.                                                */
/* Copyright (c) 2015 by Cortina Access Incorporated.                  */
/***********************************************************************/
/*
 *
 * cortina_ni.c
 *
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>
#include <miiphy.h>
//#include <environment.h>
#include <env.h>

#include "cortina_ni.h"
#ifdef CONFIG_REALTEK_10G_PON
u8 eth_initialize_flag = 0;
static int ca77xx_eth_initialize_done = 0;
#endif
static u32 reg_value = 0;
int cortina_ni_recv(struct eth_device *netdev);

/* port 0-3 are individual port connect to PHY directly */
/* port 4-7 are LAN ports connected to QSGMII PHY */
int active_port = NI_PORT_5;  /* Physical port 5 */
u32 ge_port_phy_addr;  /* PHY address connected to active port */
#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
int auto_scan_active_port = 0;
#else //RTK enable this function by default.
int auto_scan_active_port = 1;
#endif

#define HEADER_A_SIZE	8

#if CORTINA_NI_DBG /*define CORTINA_NI_DBG if individual rx,tx,init needs to be called */

struct eth_device	*dbg_dev;

#endif /* CORTINA_NI_DBG */

static struct eth_device       *curr_dev = NULL;

#define RDWRPTR_ADVANCE_ONE(x, base, max)       (((x + 1) >= (u32 *)(ca_uint)(max)) ? (u32 *)(ca_uint)base : x+1)

#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
#define CORTINA_CA_REG_READ(off)		CA_REG_READ(KSEG1_ATU_XLAT(off))
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, KSEG1_ATU_XLAT(off))
#else
#define CORTINA_CA_REG_READ(off)                CA_REG_READ(off)
#define CORTINA_CA_REG_WRITE(data, off)         CA_REG_WRITE(data, off)
#endif


#ifdef CONFIG_MULTICAST_UPGRADE //realtek, rtk
void ni_enter_promiscuous_mode(void){
       NI_HV_XRAM_CPUXRAM_CFG_t        cpuxram_cfg;

      // printf("[%s]\n", __FUNCTION__);
       /* received all packets(promiscuous mode) */
       cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
       cpuxram_cfg.bf.xram_mgmt_promisc_mode = 3;
       cpuxram_cfg.bf.rx_0_cpu_pkt_dis = 0;
       cpuxram_cfg.bf.tx_0_cpu_pkt_dis = 0;
       CORTINA_CA_REG_WRITE(cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);
}
#endif



static void ni_setup_mac_addr(void)
{
	unsigned char                   mac[6];
	NI_HV_GLB_MAC_ADDR_CFG0_t	mac_addr_cfg0;
	NI_HV_GLB_MAC_ADDR_CFG1_t	mac_addr_cfg1;
	NI_HV_PT_PORT_STATIC_CFG_t	port_static_cfg;
	NI_HV_XRAM_CPUXRAM_CFG_t 	cpuxram_cfg;

	/* parsing ethaddr and set to NI registers. */
	if (eth_env_get_enetaddr("ethaddr", mac)) {
		/* The complete MAC address consists of {MAC_ADDR0_mac_addr0[0-3], MAC_ADDR1_mac_addr1[4],
		   PT_PORT_STATIC_CFG_mac_addr6[5]}. */
		mac_addr_cfg0.bf.mac_addr0 = (mac[0]<<24) + (mac[1]<<16) + (mac[2]<<8) + mac[3];
		CORTINA_CA_REG_WRITE(mac_addr_cfg0.wrd, NI_HV_GLB_MAC_ADDR_CFG0);

		mac_addr_cfg1.wrd = 0;
		mac_addr_cfg1.bf.mac_addr1 = mac[4];
		CORTINA_CA_REG_WRITE(mac_addr_cfg1.wrd, NI_HV_GLB_MAC_ADDR_CFG1);

		port_static_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * active_port));

		port_static_cfg.bf.mac_addr6 = mac[5];
		CORTINA_CA_REG_WRITE(port_static_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * active_port));

		/* received only Broadcast and Address matched packets */
		cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
		cpuxram_cfg.bf.xram_mgmt_promisc_mode = 0;
		cpuxram_cfg.bf.rx_0_cpu_pkt_dis = 0;
		cpuxram_cfg.bf.tx_0_cpu_pkt_dis = 0;
		CORTINA_CA_REG_WRITE(cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);
	} else {
		/* received all packets(promiscuous mode) */
		cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
		cpuxram_cfg.bf.xram_mgmt_promisc_mode = 3;
		cpuxram_cfg.bf.rx_0_cpu_pkt_dis = 0;
		cpuxram_cfg.bf.tx_0_cpu_pkt_dis = 0;
		CORTINA_CA_REG_WRITE(cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);
	}
}

static void ni_enable_tx_rx(void)
{
	NI_HV_PT_RXMAC_CFG_t rxmac_cfg;
	NI_HV_PT_TXMAC_CFG_t txmac_cfg;
	//NI_HV_GLB_RXMUX_WEIGHT_RATIO_CFG0_t	weight_ratio_cfg0;
	//NI_HV_GLB_RXMUX_WEIGHT_RATIO_CFG1_t	weight_ratio_cfg1;

	/* Enable TX and RX functions */
	rxmac_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_RXMAC_CFG + APB0_NI_HV_PT_STRIDE * active_port));
	rxmac_cfg.bf.rx_en = 1;
	CORTINA_CA_REG_WRITE(rxmac_cfg.wrd, (NI_HV_PT_RXMAC_CFG + APB0_NI_HV_PT_STRIDE * active_port));

	txmac_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_TXMAC_CFG + APB0_NI_HV_PT_STRIDE * active_port));
        txmac_cfg.bf.tx_en = 1;
        CORTINA_CA_REG_WRITE(txmac_cfg.wrd, (NI_HV_PT_TXMAC_CFG + APB0_NI_HV_PT_STRIDE * active_port));

#if 0
	/* Configure RXMUX weight ratio */
	if (active_port >= NI_PORT_0 && active_port <= NI_PORT_3) {
		weight_ratio_cfg0.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMUX_WEIGHT_RATIO_CFG0);
		switch (active_port) {
			case NI_PORT_0:
				weight_ratio_cfg0.bf.port0 = 0x10;
				break;
			case NI_PORT_1:
				weight_ratio_cfg0.bf.port1 = 0x10;
				break;
			case NI_PORT_2:
				weight_ratio_cfg0.bf.port2 = 0x10;
				break;
			case NI_PORT_3:
				weight_ratio_cfg0.bf.port3 = 0x10;
				break;
		}
		CORTINA_CA_REG_WRITE(weight_ratio_cfg0.wrd, NI_HV_GLB_RXMUX_WEIGHT_RATIO_CFG0);
	}

	if (active_port >= NI_PORT_4 && active_port <= NI_PORT_7) {
		weight_ratio_cfg1.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_RXMUX_WEIGHT_RATIO_CFG1);
		switch (active_port) {
			case NI_PORT_4:
				weight_ratio_cfg1.bf.port4 = 0x10;
				break;
			case NI_PORT_5:
				weight_ratio_cfg1.bf.port5 = 0x10;
				break;
			case NI_PORT_6:
				weight_ratio_cfg1.bf.port6 = 0x10;
				break;
			case NI_PORT_7:
				weight_ratio_cfg1.bf.port7 = 0x10;
				break;
		}
		CORTINA_CA_REG_WRITE(weight_ratio_cfg1.wrd, NI_HV_GLB_RXMUX_WEIGHT_RATIO_CFG1);
	}
#endif
}

void cortina_ni_reset(void)
{
	int i;
	NI_HV_GLB_INIT_DONE_t init_done;
	NI_HV_GLB_INTF_RST_CONFIG_t  intf_rst_config;
	NI_HV_GLB_STATIC_CFG_t static_cfg;
	GLOBAL_BLOCK_RESET_t	glb_blk_reset;
#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
	GLOBAL_IO_DRIVE_CONTROL_t io_driver_control;
	GLOBAL_GIGE_PHY_t       gige_phy;
#endif
	//NI_HV_GLB_CPUXRAM_RXPKT_INTERRUPTE_t	rxpkt_int;
	//NI_HV_GLB_CPUXRAM_TXPKT_INTERRUPTE_t	txpkt_int;
	//NI_HV_GLB_INTERRUPTE_t	glb_int;
	//NI_HV_GLB_PORT_0_INTERRUPTE_t	glb_port_0_int;
	//NI_HV_PT_RX_CNTRL_CFG_t		rx_cntrl_cfg;

#ifndef CONFIG_MK_FPGA
	//CORTINA_CA_REG_WRITE(GLOBAL_IO_DRIVE_STRENGTH, 0xaab);
#endif

       /* NI global resets */
        glb_blk_reset.wrd = CORTINA_CA_REG_READ(GLOBAL_BLOCK_RESET);
        glb_blk_reset.bf.reset_ni = 1;
        CORTINA_CA_REG_WRITE(glb_blk_reset.wrd, GLOBAL_BLOCK_RESET);
        /* Remove resets */
        glb_blk_reset.bf.reset_ni = 0;
        CORTINA_CA_REG_WRITE(glb_blk_reset.wrd, GLOBAL_BLOCK_RESET);

	/* check the ready bit of NI module */
	for (i = 0; i < NI_READ_POLL_COUNT; i++) {
		init_done.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_INIT_DONE);
		if (init_done.bf.ni_init_done)
			break;
	}
	if (i == NI_READ_POLL_COUNT)
		printf("%s: NI init done not ready, init_done.wrd=0x%x!!!\n", __func__, init_done.wrd);

	/* from 2016/05/14 bit file ASIC NI default in reset mode */
#if 0
	/* NI ethernet interface reset */
	intf_rst_config.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_INTF_RST_CONFIG);
	switch (active_port) {
		case NI_PORT_0:
			intf_rst_config.bf.intf_rst_p0 = 1;
			break;
		case NI_PORT_1:
			intf_rst_config.bf.intf_rst_p1 = 1;
			break;
		case NI_PORT_2:
			intf_rst_config.bf.intf_rst_p2 = 1;
			break;
		case NI_PORT_3:
			intf_rst_config.bf.intf_rst_p3 = 1;
			break;
		case NI_PORT_4:
			intf_rst_config.bf.intf_rst_p4 = 1;
			break;
		//case NI_PORT_5:
		//	intf_rst_config.bf.intf_rst_p5 = 1;
		//	break;
		case NI_PORT_6:
		default:
			//intf_rst_config.bf.intf_rst_p6 = 1;
			break;
	}
	CORTINA_CA_REG_WRITE(intf_rst_config.wrd, NI_HV_GLB_INTF_RST_CONFIG);
#endif

	switch (active_port) {
//#if defined(CONFIG_CA77XX) || defined(CONFIG_CA8277B)
		case NI_PORT_0:
			intf_rst_config.bf.intf_rst_p0 = 0;
			intf_rst_config.bf.mac_rx_rst_p0 = 0;
			intf_rst_config.bf.mac_tx_rst_p0 = 0;
			break;
		case NI_PORT_1:
			intf_rst_config.bf.intf_rst_p1 = 0;
			intf_rst_config.bf.mac_rx_rst_p1 = 0;
			intf_rst_config.bf.mac_tx_rst_p1 = 0;
			break;
		case NI_PORT_2:
			intf_rst_config.bf.intf_rst_p2 = 0;
			intf_rst_config.bf.mac_rx_rst_p2 = 0;
			intf_rst_config.bf.mac_tx_rst_p2 = 0;
			break;
//#endif
		case NI_PORT_3:
			intf_rst_config.bf.intf_rst_p3 = 0;
			intf_rst_config.bf.mac_tx_rst_p3 = 0;
			intf_rst_config.bf.mac_rx_rst_p3 = 0;
			break;
		case NI_PORT_4:
			intf_rst_config.bf.intf_rst_p4 = 0;
			intf_rst_config.bf.mac_tx_rst_p4 = 0;
			intf_rst_config.bf.mac_rx_rst_p4 = 0;
			break;
		//case NI_PORT_5:
		//	intf_rst_config.bf.intf_rst_p5 = 0;
		//	break;
		case NI_PORT_6:
		default:
			//intf_rst_config.bf.intf_rst_p6 = 0;
			break;
	}

	CORTINA_CA_REG_WRITE(intf_rst_config.wrd, NI_HV_GLB_INTF_RST_CONFIG);


	/* Only one GMAC can connect to CPU */
	static_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_GLB_STATIC_CFG);

	static_cfg.bf.port_to_cpu = active_port;
	static_cfg.bf.txmib_mode = 1;
	static_cfg.bf.rxmib_mode = 1;

	CORTINA_CA_REG_WRITE(static_cfg.wrd, NI_HV_GLB_STATIC_CFG);

	//printf("%s: Connect port %d to CPU\n", __func__, active_port);

#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
        /* set IO driver control */
        io_driver_control.wrd = CORTINA_CA_REG_READ(GLOBAL_IO_DRIVE_CONTROL);
        io_driver_control.bf.gmac_dp = 1;
        io_driver_control.bf.gmac_dn = 1;
        io_driver_control.bf.gmac_ds = 0;
        io_driver_control.bf.gmac_mode = 2;
        CORTINA_CA_REG_WRITE(io_driver_control.wrd, GLOBAL_IO_DRIVE_CONTROL);

	/* initialize internal GPHY */
	gige_phy.wrd = 0;
	gige_phy.bf.gphy_phyrst_cen_b = 1;
	CORTINA_CA_REG_WRITE(gige_phy.wrd, GLOBAL_GIGE_PHY);
	mdelay(50);

	CORTINA_CA_REG_WRITE(0xa46, 0xd000b0fc);
	mdelay(50);
	CORTINA_CA_REG_WRITE(0x1, 0xd000b0d0);
	mdelay(100);
#endif
}


#define NI_ETH_SPEED_100 0xFFFFFFFE
#define NI_ETH_DUPLEX_FULL 0xFFFFFFD
#define PHY_MODE_MFE_MAC (1 << 12)

#define NI_RX_ENB (1 << 2)
#define NI_TX_ENB (1 << 3)
#define FLOW_CNTL_RX_DSBL (1 << 8)
#define FLOW_CNTL_TX_DSBL (1 << 12)

static ca_status_t ca_mdio_write_rgmii(ca_uint32_t addr, ca_uint32_t offset, ca_uint16_t data)
{
	PER_MDIO_ADDR_t  mdio_addr;
	PER_MDIO_CTRL_t  mdio_ctrl;
	ca_uint32_t      loop_wait = __MDIO_ACCESS_TIMEOUT; /* up to 10000 cycles*/

	mdio_addr.wrd            = 0;
	mdio_addr.bf.mdio_addr   = addr;
	mdio_addr.bf.mdio_offset = offset;
	mdio_addr.bf.mdio_rd_wr  = __MDIO_WR_FLAG;
	CORTINA_CA_REG_WRITE(mdio_addr.wrd, PER_MDIO_ADDR);
	CORTINA_CA_REG_WRITE(data, PER_MDIO_WRDATA);

#if CORTINA_NI_DBG
	printf("%s: mdio_addr.wrd=0x%x\n", __func__, mdio_addr.wrd);
#endif

	mdio_ctrl.wrd          = 0;
	mdio_ctrl.bf.mdiostart = 1;
	CORTINA_CA_REG_WRITE(mdio_ctrl.wrd, PER_MDIO_CTRL);

#if CORTINA_NI_DBG
	printf("%s: phy_addr=%d, offset=%d, data=0x%x\n", __func__, addr, offset, data);
#endif

	do {
		mdio_ctrl.wrd = CORTINA_CA_REG_READ(PER_MDIO_CTRL);
		if (mdio_ctrl.bf.mdiodone) {
			CORTINA_CA_REG_WRITE(mdio_ctrl.wrd, PER_MDIO_CTRL);
			return CA_E_OK;
		}
	} while(--loop_wait);

	printf("%s: PHY rite timeout!!!\n", __func__);
	return CA_E_TIMEOUT;
}

ca_status_t ca_mdio_write (
		CA_IN       ca_uint32_t             addr,
		CA_IN       ca_uint32_t             offset,
		CA_IN       ca_uint16_t             data)
{
#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_SATURN_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
	NI_MDIO_OPER_T	mdio_oper;
	__MDIO_ADDR_CHK(addr);

	/* the phy addr 5 is connect to RGMII */
	if (addr >= 5) {
		return  ca_mdio_write_rgmii(addr, offset, data);
	}
	else {
		mdio_oper.wrd = 0;
		mdio_oper.bf.reg_off = offset;
		mdio_oper.bf.phy_addr = addr;
		mdio_oper.bf.reg_base = CA_NI_MDIO_REG_BASE;
		CORTINA_CA_REG_WRITE(data, mdio_oper.wrd);

#if CORTINA_NI_DBG
		printf("%s: mdio_oper.wrd=0x%x, data=0x%x\n", __func__, mdio_oper.wrd, data);
#endif
		return CA_E_OK;
	}
#else
        __MDIO_ADDR_CHK(addr);

	return ca_mdio_write_rgmii(addr, offset, data);
#endif
}

static ca_status_t ca_mdio_read_rgmii(ca_uint32_t addr, ca_uint32_t offset, ca_uint16_t *data)
{
	PER_MDIO_ADDR_t  mdio_addr;
	PER_MDIO_CTRL_t  mdio_ctrl;
	PER_MDIO_RDDATA_t  read_data;
	ca_uint32_t      loop_wait = __MDIO_ACCESS_TIMEOUT;

	mdio_addr.wrd            = 0;
	mdio_addr.bf.mdio_addr   = addr;
	mdio_addr.bf.mdio_offset = offset;
	mdio_addr.bf.mdio_rd_wr  = __MDIO_RD_FLAG;
	CORTINA_CA_REG_WRITE(mdio_addr.wrd, PER_MDIO_ADDR);

	mdio_ctrl.wrd          = 0;
	mdio_ctrl.bf.mdiostart = 1;
	CORTINA_CA_REG_WRITE(mdio_ctrl.wrd, PER_MDIO_CTRL);

	do {
		mdio_ctrl.wrd = CORTINA_CA_REG_READ(PER_MDIO_CTRL);
		if (mdio_ctrl.bf.mdiodone) {
			CORTINA_CA_REG_WRITE(mdio_ctrl.wrd, PER_MDIO_CTRL);
			read_data.wrd = CORTINA_CA_REG_READ(PER_MDIO_RDDATA);
			*data = read_data.bf.mdio_rddata;
			return CA_E_OK;
		}
	} while(--loop_wait);

	printf("%s: CA_E_TIMEOUT!!\n", __func__);
	return CA_E_TIMEOUT;
}

ca_status_t ca_mdio_read (
		CA_IN       ca_uint32_t             addr,
		CA_IN       ca_uint32_t             offset,
		CA_OUT      ca_uint16_t             *data)
{
#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_SATURN_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
	NI_MDIO_OPER_T  mdio_oper;

	CA_ASSERT(data);
	__MDIO_ADDR_CHK(addr); /* support range: 1~31*/

	/* the phy addr 5 is connect to RGMII */
	if (addr >= 5) {
		return  ca_mdio_read_rgmii(addr, offset, data);
	}
	else {
		mdio_oper.wrd = 0;
		mdio_oper.bf.reg_off = offset;
		mdio_oper.bf.phy_addr = addr;
		mdio_oper.bf.reg_base = CA_NI_MDIO_REG_BASE;
		*data = CORTINA_CA_REG_READ(mdio_oper.wrd);
		return CA_E_OK;
	}
#else
        CA_ASSERT(data);
        __MDIO_ADDR_CHK(addr); /* support range: 1~31*/

	return  ca_mdio_read_rgmii(addr, offset, data);
#endif
}
#if 0 //u-boot-2015
int ca_miiphy_read(const char *devname, unsigned char addr, unsigned char reg, unsigned short *value)
{
	return ca_mdio_read(addr, reg, value);
}

int ca_miiphy_write(const char *devname, unsigned char addr, unsigned char reg, unsigned short value)
{
	return ca_mdio_write(addr, reg, value);
}
#else //u-boot-2018
int ca_miiphy_read(struct mii_dev *bus, int addr, int devad, int reg) {
	u16 value;
	int result;

	result = ca_mdio_read(addr, reg, &value);
	if (result != CA_E_OK) {
		value = result;
	}

	return result;
}

int ca_miiphy_write(struct mii_dev *bus, int addr, int devad, int reg, u16 value)
{
	return ca_mdio_write(addr, reg, value);
}
#endif

#if 0
int g2_phy_disable(int port)
{
	unsigned int addr, val;

	switch (port) {
		case 0:
			addr = GE_PORT0_PHY_ADDR;
			break;
		case 1:
			addr = GE_PORT1_PHY_ADDR;
			break;
		case 2:
			addr = GE_PORT2_PHY_ADDR;
			break;
		default:
			printf("%s: invalid port %d\n", __func__, port);
			return -1;
	}

	if (addr == 0)	/* ignore PHY address 0 */
		return 0;

	val = ca_mdio_read(addr, 0);
	val = val | (1 << 11); /* set power down */
	ca_mdio_write(addr, 0, val);
	return 0;
}
#endif

#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_RTK_V7_FPGA_PLATFORM)
static void cortina_ni_fix_gphy(void)
{
	u16     data;
	u8      phy_addr;

	for (phy_addr = 1; phy_addr < 5; phy_addr++) {

			/* Clear clock fail interrupt */
			ca_mdio_write(phy_addr, 31, 0xB90);
			ca_mdio_read(phy_addr, 19, &data);
			if (data == 0x10) {
				ca_mdio_write(phy_addr, 31, 0xB90);
				ca_mdio_read(phy_addr, 19, &data);
			printf("%s: read again phy_addr=%d, read register 19, value=0x%x\n", __func__, phy_addr, data);
			}
#ifdef CORTINA_NI_DBG
			printf("%s: phy_addr=%d, read register 19, value=0x%x\n", __func__, phy_addr, data);
#endif
	}
}
#endif

int cortina_ni_init(struct eth_device *dev, bd_t *bd)
{
	u16  vendor_id, chip_id;
	u32  phy_id;
	u16  phy_reg_value, lpagb, lpa, phy_speed, phy_duplex, speed, duplex;
	char *spd, *dup;
	NI_HV_XRAM_CPUXRAM_ADRCFG_RX_t  cpuxram_adrcfg_rx;
	NI_HV_XRAM_CPUXRAM_ADRCFG_TX_0_t  cpuxram_adrcfg_tx;
	NI_HV_XRAM_CPUXRAM_CFG_t	   cpuxram_cfg;
	NI_HV_PT_PORT_STATIC_CFG_t	port_static_cfg;
	NI_HV_PT_PORT_GLB_CFG_t	port_glb_cfg;

	/* read "ethaddr" and setup to NI regsiters */
	ni_setup_mac_addr();

	/* RX XRAM ADDRESS CONFIG (start and end address) */
	cpuxram_adrcfg_rx.wrd = 0;
	cpuxram_adrcfg_rx.bf.rx_top_addr = RX_TOP_ADDR;
	cpuxram_adrcfg_rx.bf.rx_base_addr = RX_BASE_ADDR;
	CORTINA_CA_REG_WRITE(cpuxram_adrcfg_rx.wrd, NI_HV_XRAM_CPUXRAM_ADRCFG_RX);

	/* TX XRAM ADDRESS CONFIG (start and end address) */
	cpuxram_adrcfg_tx.wrd = 0;
	cpuxram_adrcfg_tx.bf.tx_top_addr = TX_TOP_ADDR;
	cpuxram_adrcfg_tx.bf.tx_base_addr = TX_BASE_ADDR;
	CORTINA_CA_REG_WRITE(cpuxram_adrcfg_tx.wrd, NI_HV_XRAM_CPUXRAM_ADRCFG_TX_0);

	ca_mdio_read(ge_port_phy_addr, 0x02, &vendor_id);
	ca_mdio_read(ge_port_phy_addr, 0x03, &chip_id);
	phy_id = (vendor_id << 16) | chip_id;

#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_RTK_V7_FPGA_PLATFORM)
        /* workaround to fix GPHY fail */
	if ((phy_id & PHY_ID_MASK) == PHY_ID_RTL8211_G3_ASIC) {
		cortina_ni_fix_gphy();
	}
#endif

	ca_mdio_read(ge_port_phy_addr, 0x0a, &lpagb);  /* PHY GB status */
	ca_mdio_read(ge_port_phy_addr, 0x09, &phy_reg_value); /* PHY GB control */
	lpagb &= phy_reg_value << 2;

	ca_mdio_read(ge_port_phy_addr, 0x05, &lpa); /* Link Partner Ability */
	ca_mdio_read(ge_port_phy_addr, 0x04, &phy_reg_value); /* PHY Advertisement */
	lpa &= phy_reg_value;

	/* phy_speed 0: 10Mbps, 1: 100Mbps, 2: 1000Mbps */
	/* duplex 0: half duplex, 1: full duplex */
	phy_speed = 0;
	phy_duplex = 0;
	if (lpagb & (3 << 10)) {  	/* 1000Mbps */
		phy_speed = 2;
		if (lpagb & (1 << 11))  /* 1000Mbps full */
			duplex = 1;
	} else if (lpa & (3 << 7)) {  	/* 100Mbps */
		phy_speed = 1;
		if (lpa & (1 << 8))  	/* 100Mbps full */
			phy_duplex = 1;
	} else if (lpa & (1 << 6)) { 	/* 10Mbps full */
		phy_duplex = 1;
	}

	switch (phy_speed) {
		default:
		case 0:
			spd = "10Mbps";
			break;
		case 1:
			spd = "100Mbps";
			break;
		case 2:
			spd = "1000Mbps";
			break;
	}

	if (duplex == 1)
		dup = "full duplex";
	else
		dup = "half duplex";

	printf("PHY ID 0x%08X %s %s\n", phy_id, spd, dup);

	switch (phy_id & PHY_ID_MASK) {
		case PHY_ID_RTL8214:
			port_static_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * active_port));
			port_static_cfg.bf.int_cfg = GE_MAC_INTF_QSGMII_1000;   /* QSGMII_GE */
			CORTINA_CA_REG_WRITE(port_static_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * active_port));
			break;
		case PHY_ID_RTL8211:
#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
		case PHY_ID_RTL8211_G3_ASIC:
#endif
#ifdef CONFIG_TARGET_SATURN_ASIC
                case PHY_ID_RTL8211_SATURN_ASIC:
#endif
		default:
			/*
			 * Configuration for Management Ethernet
			 * Interface:
			 * - RGMII 1000 mode or RGMII 100 mode
			 * - MAC mode
			 */
			port_static_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * active_port));
			if (phy_speed == 2 /* 1000Mbps */) {
				/* port 4 connects to RGMII PHY */
				if (ge_port_phy_addr == 5)
					port_static_cfg.bf.int_cfg = GE_MAC_INTF_RGMII_1000;
				else
					port_static_cfg.bf.int_cfg = GE_MAC_INTF_GMII;
			}
			else {
#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_SATURN_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
				/* port 4 connects to RGMII PHY */
				if (ge_port_phy_addr == 5) {
					port_static_cfg.bf.int_cfg = GE_MAC_INTF_RGMII_100;
				}
				else {
					port_static_cfg.bf.int_cfg = GE_MAC_INTF_MII;
				}
#else
				port_static_cfg.bf.int_cfg = GE_MAC_INTF_RGMII_100;
#endif
			}
			CORTINA_CA_REG_WRITE(port_static_cfg.wrd, (NI_HV_PT_PORT_STATIC_CFG + APB0_NI_HV_PT_STRIDE * active_port));
			break;
	}


	port_glb_cfg.wrd = CORTINA_CA_REG_READ((NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * active_port));
	if (phy_speed == 0)  /* 10Mbps */
		speed = 1;
	else
		speed = 0;
	if (phy_duplex == 0)  /* half duplex */
		duplex = 1;
	else
		duplex = 0;
	port_glb_cfg.bf.speed = speed;
	port_glb_cfg.bf.duplex = duplex;
	CORTINA_CA_REG_WRITE(port_glb_cfg.wrd, (NI_HV_PT_PORT_GLB_CFG + APB0_NI_HV_PT_STRIDE * active_port));

#if FOR_DEBUG
	/* Enable MFE ethernet interface */
	reg_value= CORTINA_CA_REG_READ(NI_TOP_NI_INTF_RST_CONFIG);
	reg_value= reg_value& ~(INTF_RST_GE);
	CORTINA_CA_REG_WRITE(reg_value, NI_TOP_NI_INTF_RST_CONFIG);
#endif

	/* Need to toggle the tx and rx cpu_pkt_dis bit */
	/* after changing Address config register.      */
	cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
	////cpuxram_cfg.bf.xram_mgmt_promisc_mode = 3;
	cpuxram_cfg.bf.rx_0_cpu_pkt_dis = 1;
	cpuxram_cfg.bf.tx_0_cpu_pkt_dis = 1;
	CORTINA_CA_REG_WRITE(cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);

	cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
	cpuxram_cfg.bf.rx_0_cpu_pkt_dis = 0;
	cpuxram_cfg.bf.tx_0_cpu_pkt_dis = 0;
	CORTINA_CA_REG_WRITE(cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);

#if 0
	cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
	printf("%s: cpuxram_cfg.wrd=0x%x, NI_HV_XRAM_CPUXRAM_CFG=0x%x\n", __func__, cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);
#endif

	ni_enable_tx_rx();

	return 0;
}


void cortina_ni_check_rx_packet(void)
{
	static int first_time = 1;
	NI_HV_XRAM_CPUXRAM_CFG_t        cpuxram_cfg;

	if (first_time) {
		/* received all kind of packets */
		cpuxram_cfg.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CFG);
		cpuxram_cfg.bf.xram_mgmt_promisc_mode = 3;
		cpuxram_cfg.bf.rx_0_cpu_pkt_dis = 0;
		cpuxram_cfg.bf.tx_0_cpu_pkt_dis = 0;
		CORTINA_CA_REG_WRITE(cpuxram_cfg.wrd, NI_HV_XRAM_CPUXRAM_CFG);
		first_time = 0;
	}

	cortina_ni_recv(curr_dev);
}
#define RTK_NI_RECV_PATCH 1
#define RTK_PKT_BUDGET  512
/*********************************************
 * Packet receive routine from Management FE
 * Expects a previously allocated buffer and
 * fills the length
 * Retruns 0 on success -1 on failure
 *******************************************/
int cortina_ni_recv(struct eth_device *netdev)
{
	struct cortina_ni_priv 	*priv = (struct cortina_ni_priv *)(netdev->priv);
	NI_HEADER_X_T		header_x;
	u32			pktlen=0;
	u32 			sw_rx_rd_ptr;
	u32 			hw_rx_wr_ptr;
	volatile u32 		*rx_xram_ptr;
#ifdef CORTINA_NI_DBG
	int			blk_num;
#endif
	int			loop;
	//static unsigned char   	pkt_buf[2048];
	u32	 		*data_ptr;
#ifdef CORTINA_NI_DBG
	u8	 		*ptr;
#endif


#ifdef  RTK_NI_RECV_PATCH
    u32 pkt_cnt = 0;
#endif
	NI_PACKET_STATUS_T	packet_status;
	NI_HV_XRAM_CPUXRAM_CPU_STA_RX_0_t cpuxram_cpu_sta_rx;
	NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0_t cpuxram_cpu_cfg_rx;
	int index = 0;

	/* get the hw write pointer */
	cpuxram_cpu_sta_rx.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_STA_RX_0);
	hw_rx_wr_ptr = cpuxram_cpu_sta_rx.bf.pkt_wr_ptr;

	/* get the sw read pointer */
	cpuxram_cpu_cfg_rx.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);
	sw_rx_rd_ptr = cpuxram_cpu_cfg_rx.bf.pkt_rd_ptr;

#if CORTINA_NI_DBG
	printf("%s: NI_HV_XRAM_CPUXRAM_CPU_STA_RX_0=0x%x, NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0=0x%x\n", __func__,
		NI_HV_XRAM_CPUXRAM_CPU_STA_RX_0, NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);

	printf("%s : RX hw_wr_ptr=%d, sw_rd_ptr=%d\n", __func__, hw_rx_wr_ptr, sw_rx_rd_ptr);
#endif

	while (sw_rx_rd_ptr != hw_rx_wr_ptr) {

		/* Point to the absolute memory address of XRAM where read pointer is */
		rx_xram_ptr = (u32 *)((ca_uint)NI_XRAM_BASE + sw_rx_rd_ptr * 8);

		/* Wrap around if required */
		if (rx_xram_ptr >= (u32 *)(ca_uint)(priv->rx_xram_end_adr)) {
			rx_xram_ptr = (u32 *)(ca_uint)(priv->rx_xram_base_adr);
		}

		/* Checking header XR. Do not update the read pointer yet */
		//rx_xram_ptr++; 	/* skip unused 32-bit in Header XR */
		rx_xram_ptr = RDWRPTR_ADVANCE_ONE(rx_xram_ptr, priv->rx_xram_base_adr, priv->rx_xram_end_adr);

		header_x = (NI_HEADER_X_T)(*rx_xram_ptr);   /* Header XR [31:0] */

		if (*rx_xram_ptr == 0xffffffff)
			printf("%s: XRAM Error !\n", __func__);
#if CORTINA_NI_DBG
		printf("%s : RX next link %x(%d) \n" ,__func__,	header_x.bf.next_link,header_x.bf.next_link);
		printf("%s : bytes_valid %x\n", __func__, header_x.bf.bytes_valid);
#endif

		if (header_x.bf.ownership == 0) {

			//rx_xram_ptr++;	/* point to Packet status [31:0] */
			rx_xram_ptr = RDWRPTR_ADVANCE_ONE(rx_xram_ptr, priv->rx_xram_base_adr, priv->rx_xram_end_adr);

			packet_status = (NI_PACKET_STATUS_T)(*rx_xram_ptr);

#if CORTINA_NI_DBG
			printf("%s: packet_status=0x%x\n", __func__, packet_status);
#endif
			if (packet_status.bf.valid == 0) {
#if CORTINA_NI_DBG
				printf("%s: Invalid Packet !!, Packet status=0x%x, header_x.bf.next_link=%d\n", __func__, packet_status.wrd, header_x.bf.next_link);
#endif

				/* Update the software read pointer */
				CORTINA_CA_REG_WRITE(header_x.bf.next_link, NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);
				return 0;
			}

			if (packet_status.bf.drop || packet_status.bf.runt || packet_status.bf.oversize ||
					packet_status.bf.jabber || packet_status.bf.crc_error || packet_status.bf.jumbo) {
#if CORTINA_NI_DBG
				printf("%s: Error Packet !! Packet status=0x%x, header_x.bf.next_link=%d\n", __func__, packet_status.wrd, header_x.bf.next_link);
#endif

				/* Update the software read pointer */
				CORTINA_CA_REG_WRITE(header_x.bf.next_link, NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);
				return 0;
			}

			/* check whether packet size is larger than 1514 */
			if (packet_status.bf.packet_size > 1518) {
#if CORTINA_NI_DBG
				printf("%s: Error Packet !! Packet size=%d, larger than 1518, Packet status=0x%x, header_x.bf.next_link=%d\n", 						 __func__, packet_status.bf.packet_size, packet_status.wrd, header_x.bf.next_link);
#endif

				/* Update the software read pointer */
				CORTINA_CA_REG_WRITE(header_x.bf.next_link, NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);
				return 0;

			}

#if 0
			/* Wrap around if required */
			if (rx_xram_ptr >= (u32 *)(ca_uint)(priv->rx_xram_end_adr)) {
				rx_xram_ptr = (u32 *)(ca_uint)(priv->rx_xram_base_adr);
			}

			rx_xram_ptr++;  /* skip Packet status [31:0] */

			/* Wrap around if required */
			if (rx_xram_ptr >= (u32 *)(ca_uint)(priv->rx_xram_end_adr)) {
				rx_xram_ptr = (u32 *)(ca_uint)(priv->rx_xram_base_adr);
			}
#endif
			rx_xram_ptr = RDWRPTR_ADVANCE_ONE(rx_xram_ptr, priv->rx_xram_base_adr, priv->rx_xram_end_adr);

			pktlen = packet_status.bf.packet_size;

#if CORTINA_NI_DBG
			printf("%s : rx packet length = %d\n", __func__, packet_status.bf.packet_size);
#endif

#if 0
			rx_xram_ptr++;	/* point to data address */

			/* Wrap around if required */
			if (rx_xram_ptr >= (u32 *)(ca_uint)(priv->rx_xram_end_adr)) {
				rx_xram_ptr = (u32 *)(ca_uint)(priv->rx_xram_base_adr);
			}
#endif
			rx_xram_ptr = RDWRPTR_ADVANCE_ONE(rx_xram_ptr, priv->rx_xram_base_adr, priv->rx_xram_end_adr);

			//data_ptr = (u32 *)pkt_buf;
			data_ptr = (u32 *)net_rx_packets[0];

			/* Read out the packet */
			/* Data is in little endian form in the XRAM */

			/* Send the packet to upper layer */

#if CORTINA_NI_DBG
			printf("%s: packet data[]=", __func__);
#endif

			for(loop=0; loop<= pktlen/4; loop++) {

#if CORTINA_NI_DBG
				ptr = rx_xram_ptr;
				if (loop < 10)
				printf("[0x%x]-[0x%x]-[0x%x]-[0x%x]", ptr[0], ptr[1], ptr[2], ptr[3]);
#endif
				*data_ptr++ = *rx_xram_ptr++;
				/* Wrap around if required */
				if (rx_xram_ptr >= (u32 *)(ca_uint)(priv->rx_xram_end_adr)) {
					rx_xram_ptr = (u32 *)(ca_uint)(priv->rx_xram_base_adr);
				}
			}
#if CORTINA_NI_DBG
				printf("\n");
#endif
			//memcpy((void*)net_rx_packets[0], pkt_buf, pktlen);
#if CORTINA_NI_DBG
			blk_num=net_rx_packets[index][0x2c]*255+net_rx_packets[index][0x2d];
			printf("%s: tftp block number=%d\n",__func__,blk_num);
#endif

				/* Update the software read pointer */
			CORTINA_CA_REG_WRITE(header_x.bf.next_link, NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);

			net_process_received_packet(net_rx_packets[index], pktlen-4);
			if (++index >= PKTBUFSRX) {
				index = 0;
			}

#ifdef RTK_NI_RECV_PATCH
			pkt_cnt++;
#endif
		}
#ifdef RTK_NI_RECV_PATCH
		if(pkt_cnt >= RTK_PKT_BUDGET){
#if CORTINA_NI_DBG
			printf("%s: Received PKT Count(%u) > RTK_PKT_BUDGET(%u)\n", __func__, pkt_cnt, RTK_PKT_BUDGET);
#endif
			return 0;
		}
#endif
		/* get the hw write pointer */
		cpuxram_cpu_sta_rx.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_STA_RX_0);
		hw_rx_wr_ptr = cpuxram_cpu_sta_rx.bf.pkt_wr_ptr;

		/* get the sw read pointer */
		sw_rx_rd_ptr = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_CFG_RX_0);
	}
	return 0;
}


/* LITTLE_ENDIAN */

static u32 calc_crc(u32 crc, u8 const *p, u32 len)
{
	int i;

	while (len--) {
		crc ^= *p++;
		for (i = 0; i < 8; i++) {
			crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
		}
	}
	return crc;
}

static int cortina_ni_send(struct eth_device *dev, void *packet, int length)
{
	struct cortina_ni_priv *priv = (struct cortina_ni_priv *)(dev->priv);
	u32	hw_tx_rd_ptr;
	u32	sw_tx_wr_ptr;
	unsigned int	new_pkt_len;
	unsigned char 	valid_bytes=0;
	u32	*tx_xram_ptr;
	u16  next_link = 0;
	unsigned char	*pkt_buf_ptr;
	unsigned int	loop;
	u32 	crc32;
	//unsigned int 	hdr_xt=0;
	NI_HEADER_X_T	hdr_xt;
	int  		pad=0;
	static unsigned char   pkt_buf[2048];
#ifdef CONFIG_RTK_V7_FPGA_PLATFORM //workaround writing 8 byte
	u64 	*data_ptr;
#else
	u32 	*data_ptr;
#endif
	//NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0_t	cpuxram_cpu_stat_tx;
	NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0_t	cpuxram_cpu_cfg_tx;
#if CORTINA_NI_DBG
	u8 *ptr;
#endif

	if ((NULL == packet) || ( length > 2032)) {
		return -1;
	}

	/* Get the hardware read pointer */
	//cpuxram_cpu_stat_tx.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0);
	//hw_tx_rd_ptr = cpuxram_cpu_stat_tx.bf.pkt_rd_ptr;
	hw_tx_rd_ptr = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0);

	/* Get the software write pointer */
	//cpuxram_cpu_cfg_tx.wrd = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0);
	//sw_tx_wr_ptr = cpuxram_cpu_cfg_tx.bf.pkt_wr_ptr;
	sw_tx_wr_ptr = CORTINA_CA_REG_READ(NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0);

#if CORTINA_NI_DBG
#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
	printf("%s: NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0=0x%x, NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0=0x%x\n", __func__,
			KSEG1_ATU_XLAT(NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0), KSEG1_ATU_XLAT(NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0));
#else
	printf("%s: NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0=0x%x, NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0=0x%x\n", __func__,
			NI_HV_XRAM_CPUXRAM_CPU_STAT_TX_0, NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0);
#endif
	printf("%s : hw_tx_rd_ptr = %d\n", __func__, hw_tx_rd_ptr);
	printf("%s : sw_tx_wr_ptr = %d\n", __func__, sw_tx_wr_ptr);
#endif

	if (hw_tx_rd_ptr != sw_tx_wr_ptr)
	{
		printf("%s: Tx FIFO is not available!\n", __func__);
		return 1;
	}

	/* a workaround on 2015/10/01 the packet size+CRC should be 8-byte alignment */
	if (((length+4) % 8) != 0)
		length += (8 - ((length+4) % 8));

	memset(pkt_buf, 0x00, sizeof(pkt_buf));

	/* add 8-byte header_A at the beginning of packet */
	//memcpy(&(pkt_buf[0]), (const void *)packet, 8);
	memcpy(&(pkt_buf[HEADER_A_SIZE]), (const void *)packet, length);

	pad = 64 - (length + 4);	/* if packet length < 60 */
	pad = (pad < 0) ? 0 : pad;

#if CORTINA_NI_DBG
	printf("%s: length=%d, pad=%d\n", __func__, length, pad);
#endif

	new_pkt_len = length + pad ;	/* new packet length */

	pkt_buf_ptr = (unsigned char *)pkt_buf;

	/* Calculate the CRC32 */
	/* skip 8-byte header_A */
	crc32 = ~(calc_crc(~0, (u8 *)(pkt_buf_ptr + HEADER_A_SIZE), new_pkt_len));

#if CORTINA_NI_DBG
	printf("%s: crc32 is 0x%x \n", __func__, crc32);
	printf("%s: ~crc32 is 0x%x \n", __func__, ~crc32);
	printf("%s: pkt len %d \n", __func__, new_pkt_len);
#endif
	/* should add 8-byte header_! */
	/* CRC will re-calculated by hardware */
	memcpy ((pkt_buf_ptr+new_pkt_len+HEADER_A_SIZE), (u8 *)(&crc32), sizeof(crc32));
	new_pkt_len = new_pkt_len + 4;	/* add CRC */

	valid_bytes = new_pkt_len % 8;
	valid_bytes = valid_bytes ? valid_bytes : 0;

#if CORTINA_NI_DBG
	printf("%s: valid_bytes %d\n", __func__, valid_bytes);
#endif

	/* should add 8-byte headerA */
	next_link = sw_tx_wr_ptr + (new_pkt_len+7+HEADER_A_SIZE) / 8 ; /* for headr XT */
	next_link = next_link + 1; /* add header */
	/* Wrap around if required */
	if(next_link > priv->tx_xram_end) {
		next_link = priv->tx_xram_start + (next_link - (priv->tx_xram_end+1));
	}

#if CORTINA_NI_DBG
	printf("%s: TX next_link %x\n", __func__, next_link);
#endif

	hdr_xt.wrd = 0;
	hdr_xt.bf.ownership = 1;
	hdr_xt.bf.bytes_valid = valid_bytes;
	hdr_xt.bf.next_link = next_link;

	tx_xram_ptr = (u32 *)((ca_uint)NI_XRAM_BASE + sw_tx_wr_ptr * 8);

	/* Wrap around if required */
	if (tx_xram_ptr >= (u32 *)(ca_uint)(priv->tx_xram_end_adr)) {
		tx_xram_ptr = (u32 *)(ca_uint)(priv->tx_xram_base_adr);
	}

#if  0
	/* skip first 4 bytes before copying the headerXT */
	tx_xram_ptr++;

	/* Wrap around if required */
	if (tx_xram_ptr >= (u32 *)(ca_uint)(priv->tx_xram_end_adr)) {
		tx_xram_ptr = (u32 *)(ca_uint)(priv->tx_xram_base_adr);
	}
#endif
	tx_xram_ptr = RDWRPTR_ADVANCE_ONE(tx_xram_ptr, priv->tx_xram_base_adr, priv->tx_xram_end_adr);

	*tx_xram_ptr = hdr_xt.wrd;

#if  0
	tx_xram_ptr++;

	/* Wrap around if required */
	if (tx_xram_ptr >= (u32 *)(ca_uint)(priv->tx_xram_end_adr)) {
		tx_xram_ptr = (u32 *)(ca_uint)(priv->tx_xram_base_adr);
	}
#endif
	tx_xram_ptr = RDWRPTR_ADVANCE_ONE(tx_xram_ptr, priv->tx_xram_base_adr, priv->tx_xram_end_adr);

	/* Now to copy the data . The first byte on the line goes first */
#ifdef CONFIG_RTK_V7_FPGA_PLATFORM //workaround writing 8 byte
	data_ptr = (u64 *)pkt_buf_ptr;
#else
	data_ptr = (u32 *)pkt_buf_ptr;
#endif

#if CORTINA_NI_DBG
	printf("%s: packet data[]=", __func__);
#endif

	/* copy header_A to XRAM */
#ifdef CONFIG_RTK_V7_FPGA_PLATFORM //workaround writing 8 byte
	for (loop = 0; loop <= (new_pkt_len + HEADER_A_SIZE) / 8; loop++)
#else
	for (loop = 0; loop <= (new_pkt_len + HEADER_A_SIZE) / 4; loop++)
#endif
	{

#if CORTINA_NI_DBG
		ptr = data_ptr;
		if ((loop % 4) == 0)
			printf("\n");
		printf("[0x%x]-[0x%x]-[0x%x]-[0x%x]-", ptr[0], ptr[1], ptr[2], ptr[3]);
#endif

#if 0
		*tx_xram_ptr++ = *data_ptr++;
		/* Wrap around if required */
		if (tx_xram_ptr >= (u32 *)(ca_uint)(priv->tx_xram_end_adr)) {
			tx_xram_ptr = (u32 *)(ca_uint)(priv->tx_xram_base_adr);
		}
#endif
#ifdef CONFIG_RTK_V7_FPGA_PLATFORM //workaround writing 8 byte
		*((u64 *)tx_xram_ptr) = *data_ptr++;
		tx_xram_ptr = RDWRPTR_ADVANCE_ONE(tx_xram_ptr, priv->tx_xram_base_adr, priv->tx_xram_end_adr);
		tx_xram_ptr = RDWRPTR_ADVANCE_ONE(tx_xram_ptr, priv->tx_xram_base_adr, priv->tx_xram_end_adr);
#else
		*tx_xram_ptr = *data_ptr++;
		tx_xram_ptr = RDWRPTR_ADVANCE_ONE(tx_xram_ptr, priv->tx_xram_base_adr, priv->tx_xram_end_adr);
#endif
	}
#if CORTINA_NI_DBG
	printf("\n");
#endif

	/* Publish the write pointer */
	cpuxram_cpu_cfg_tx.bf.pkt_wr_ptr = next_link;
	CORTINA_CA_REG_WRITE(cpuxram_cpu_cfg_tx.wrd, NI_HV_XRAM_CPUXRAM_CPU_CFG_TX_0);

	return 0;
}

void cortina_ni_halt(struct eth_device *netdev)
{
#if FOR_DEBUG
	/* MFE MAC configuration Disable tx and rx */
	reg_value= CORTINA_CA_REG_READ(NI_TOP_NI_ETH_MAC_CONFIG0_0_MFE);
	reg_value= reg_value& ~(NI_RX_ENB);
	reg_value= reg_value& ~(NI_TX_ENB);
	CORTINA_CA_REG_WRITE(reg_value, NI_TOP_NI_ETH_MAC_CONFIG0_0_MFE);

	/* Disable MFE ethernet interface */
	reg_value= CORTINA_CA_REG_READ(NI_TOP_NI_INTF_RST_CONFIG);
	reg_value= reg_value| (INTF_RST_GE1) ;
	CORTINA_CA_REG_WRITE(reg_value, NI_TOP_NI_INTF_RST_CONFIG);
#endif
	return;
}

#define RTL8214_INIT_REG_COUNT		58
static u32 rtl8214_init_reg_val[RTL8214_INIT_REG_COUNT] = {
	0x6602, 0x84D7, 0x6601, 0x0540, 0x6600, 0x00C0,
	0x6602, 0xF994, 0x6601, 0x0541, 0x6600, 0x00C0,
	0x6602, 0x2DA3, 0x6601, 0x0542, 0x6600, 0x00C0,
	0x6602, 0x3960, 0x6601, 0x0543, 0x6600, 0x00C0,
	0x6602, 0x9728, 0x6601, 0x0544, 0x6600, 0x00C0,
	0x6602, 0xF83F, 0x6601, 0x0545, 0x6600, 0x00C0,
	0x6602, 0x9D85, 0x6601, 0x0423, 0x6600, 0x00C0,
	0x6602, 0xD810, 0x6601, 0x0424, 0x6600, 0x00C0,
	0x1D11, 0x1506,
	0x1D12, 0x0800,
	0x6602, 0xC3FA, 0x6601, 0x002E, 0x6600, 0x00C0
};

#ifdef CONFIG_TARGET_PRESIDIO_ASIC
/* move to LED driver */
#if 0
/* initialize LEDs for ethernet link and traffic lights */
static void ca77xx_init_led(void)
{
	int i;
	GLOBAL_PIN_MUX_t	pin_mux;
	GLOBAL_LED_CONTROL_t	led_control;
	GLOBAL_LED_CONFIG_0_t	led_config0;

	pin_mux.wrd = CORTINA_CA_REG_READ(GLOBAL_PIN_MUX);
	pin_mux.bf.iomux_led_enable = 1;
	CORTINA_CA_REG_WRITE(pin_mux.wrd, GLOBAL_PIN_MUX);

	/* LED control & config - led 0 ~ 7 for rx of port 0 ~ 7, led 8 ~ 15 for tx of port 0 ~ 7 with 256ms blinking */
	led_control.wrd = CORTINA_CA_REG_READ(GLOBAL_LED_CONTROL);
	led_control.bf.blink_rate_1 = 0x0f;
	led_control.bf.blink_rate_2 = 0x1f;
	CORTINA_CA_REG_WRITE(led_control.wrd, GLOBAL_LED_CONTROL);

	for (i = 0; i < 8; i++) {
		led_config0.wrd = CORTINA_CA_REG_READ(GLOBAL_LED_CONFIG_0 + i * 4);
		led_config0.bf.led_event_blink = 1;
		led_config0.bf.led_off_val = 1;
		led_config0.bf.led_port = i;
		CORTINA_CA_REG_WRITE(led_config0.wrd, GLOBAL_LED_CONFIG_0 + i * 4);
	}

	for (i = 0; i < 8; i++) {
		led_config0.wrd = CORTINA_CA_REG_READ(GLOBAL_LED_CONFIG_8 + i * 4);
		led_config0.bf.led_event_blink = 2;
		led_config0.bf.led_off_val = 1;
		led_config0.bf.led_port = i;
		CORTINA_CA_REG_WRITE(led_config0.wrd, GLOBAL_LED_CONFIG_8 + i * 4);
	}
}
#endif
#endif

#if defined(CONFIG_TARGET_VENUS)
static void __internal_phy_init(int reset_phy)
{
	u16 phy_addr;
	u16 data;
	u32 start_time;

	/* GPHY clock rg_clr_clk_die_sys=0/rg_auto_clr_clk_die[10]=1 */
	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
	ca_mdio_write(phy_addr, 31, 0x0C40);
	ca_mdio_write(phy_addr, 17, 0x0480);
	}

	/* port 2 PLL port AIF correction */
	phy_addr = 3;
	ca_mdio_write(phy_addr, 31, 0x0BC6);
	ca_mdio_write(phy_addr, 16, 0x0053);
	ca_mdio_write(phy_addr, 18, 0x4003);
	ca_mdio_write(phy_addr, 22, 0x7E01);
	ca_mdio_write(phy_addr, 23, 0);

	/* GIGA port AIF correction */
	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
	ca_mdio_write(phy_addr, 31, 0x0BC0);
	ca_mdio_write(phy_addr, 19, 0x01C0);
	ca_mdio_write(phy_addr, 20, 0x3C61);
	ca_mdio_write(phy_addr, 21, 0x8022);
	ca_mdio_write(phy_addr, 23, 0x0441);
	ca_mdio_write(phy_addr, 31, 0x0BC1);
	ca_mdio_write(phy_addr, 16, 0x1328);
	ca_mdio_write(phy_addr, 17, 0xF0B0);
	ca_mdio_write(phy_addr, 18, 0x0E00);
	}
	mdelay(1);

	/* check clk_fail = 0, read phy_fatal_err_int_flag & rg_auto_clr_clk_die[10]=0, power-on & reset GPHY */
	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
	ca_mdio_write(phy_addr, 31, 0x0C40);
	ca_mdio_read(phy_addr,  18, &data);
	if (data & 1)
		printf("%s: GPHY clock failed!!\n", __func__);
	ca_mdio_write(phy_addr, 31, 0x0B90);
	ca_mdio_read(phy_addr,  19, &data);
	ca_mdio_write(phy_addr, 31, 0x0B90);
	ca_mdio_read(phy_addr,  19, &data);
	mdelay(1);
	ca_mdio_write(phy_addr, 31, 0x0C40);
	ca_mdio_write(phy_addr, 17, 0x0080);
	mdelay(1);
	ca_mdio_write(phy_addr, 31, 0x0A42);

	/* VCM_P0/1/2/3_GIGA_L=2'b10, REG_REF_P0/1/2/3_GIGA_L =3'b100 */
	ca_mdio_write(phy_addr, 31, 0x0BCC);
	ca_mdio_write(phy_addr, 16, 0xA508);

	if (reset_phy) {
		ca_mdio_write(phy_addr,  0, 0x9140);

		start_time = get_timer(0);
		while (get_timer(start_time) < 3000) {
			ca_mdio_read(phy_addr,   0, &data);
			if (!(data & 0x8000)) {
				printf("%s: GPHY addr=%d, reset completed!!\n", __func__, phy_addr);
				break;
			}
		}
		if (data & 0x8000)
			printf("%s: GPHY addr=%d, reset timeout!!\n", __func__, phy_addr);
	}
	else
		ca_mdio_write(phy_addr,  0, 0x1140);

	mdelay(100);
}
}
#endif

#if (defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
static void __internal_phy_init(int reset_phy)
{
	u16 phy_addr;
	/* check clk_fail = 0, read phy_fatal_err_int_flag & rg_auto_clr_clk_die[10]=0, power-on & reset GPHY */
	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
		ca_mdio_write(phy_addr,  0, 0x1140);
		mdelay(100);
	}
}
#endif

#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
static void ca_ni_internal_phy_init(void)
{
#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
	u8 phy_addr;
	/* should initialize 4 GPHYs at once */
	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
		ca_mdio_write(phy_addr, 31, 0x0BC6);
		ca_mdio_write(phy_addr, 16, 0x0053);
		ca_mdio_write(phy_addr, 18, 0x4003);
		ca_mdio_write(phy_addr, 22, 0x7e01);
		ca_mdio_write(phy_addr, 31, 0x0A42);
		ca_mdio_write(phy_addr, 31, 0x0A40);
		ca_mdio_write(phy_addr,  0, 0x1140);
	}

	/* workaround to fix GPHY fail */
	cortina_ni_fix_gphy();

#else
	/* should initialize 4 GPHYs at once */
	 __internal_phy_init(0);
#endif
}
#endif

#if defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
#ifdef CONFIG_CORTINA_LED

#ifdef CONFIG_RTK_LED_STYLE_1
extern uint8_t port2led[8][1];
/* RTK one port only use 1 LED */
static void ca77xx_ni_led(int port, int sw_on)
{
        if (sw_on) {
                /* turn on led light */
                __led_set(1 << port2led[port][0], STATUS_LED_ON);
        }
        else {
                /* turn off led light */
                __led_set(1 << port2led[port][0], STATUS_LED_OFF);
        }
}
#else
extern uint8_t port2led[8][2];

static void ca77xx_ni_led(int port, int sw_on)
{
        if (sw_on) {
                /* turn on led light */
                __led_set(1 << port2led[port][0], STATUS_LED_ON);
                __led_set(1 << port2led[port][1], STATUS_LED_ON);
        }
        else {
                /* turn off led light */
                __led_set(1 << port2led[port][0], STATUS_LED_OFF);
                __led_set(1 << port2led[port][1], STATUS_LED_OFF);
        }
}
#endif
#else //Not define CONFIG_CORTINA_LED,  This function is Cortin Hw Led driver

#define ca77xx_ni_led(x,y)

#endif //CONFIG_CORTINA_LED

#define AUTO_SCAN_PHY_TIMEOUT	1200		/* 1.2 s */

void ca77xx_ni_scan_phy_link(void)
{
	u8      phy_addr;
	static u32     start_time = 0;
	ca_uint16_t data;
	int port;
	int found_active_port = 0;

	/* if etherent not initialzed do nothing */
	if (curr_dev == NULL)
		return;

	if (start_time == 0) {
		start_time = get_timer(0);
	}
	else {
		/* scan GPHY link status per second */
		if (get_timer(start_time) > AUTO_SCAN_PHY_TIMEOUT) {
			if (auto_scan_active_port) {
				/* search for the first link PHY act as active port */
				for (phy_addr = 1; phy_addr < 5; phy_addr++) {
					port = phy_addr - 1;
					ca_mdio_read(phy_addr, 1, &data);
					if (data & 0x04) {
						if (found_active_port == 0) {
							/* only apply new active_port when port changed */
							if (phy_addr != ge_port_phy_addr) {
								ge_port_phy_addr = phy_addr;
								active_port = port;
								cortina_ni_reset();
								ca77xx_ni_led(port, 1);
								printf("active port has been changed to port %d\n", active_port);
							}
							else {
								ca77xx_ni_led(port, 1);
							}
							found_active_port = 1;
						}
						else
							ca77xx_ni_led(port, 1);
					}
					else {
						ca77xx_ni_led(port, 0);
					}
				}
			}
			else {
				ca_mdio_read(ge_port_phy_addr, 1, &data);
				if (data & 0x04) {
					ca77xx_ni_led(active_port, 1);
				}
				else {
					ca77xx_ni_led(active_port, 0);
				}
			}
			start_time = 0;
		}
	}
}

/* auto scan the first link up port as active_port */
#define AUTO_SCAN_TIMEOUT	3000		/* 3 seconds */

static void ca77xx_ni_auto_scan_active_port(void)
{
	u8      phy_addr;
	u32 	start_time;
	ca_uint16_t data;

	#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
	/* should initialize 4 GPHYs at once */
	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
		ca_mdio_write(phy_addr, 31, 0x0BC6);
		ca_mdio_write(phy_addr, 16, 0x0053);
		ca_mdio_write(phy_addr, 18, 0x4003);
		ca_mdio_write(phy_addr, 22, 0x7e01);
		ca_mdio_write(phy_addr, 31, 0x0A42);
		ca_mdio_write(phy_addr, 31, 0x0A40);
		ca_mdio_write(phy_addr,  0, 0x1140);
	}
	/* workaround to fix GPHY fail */
	cortina_ni_fix_gphy();
#else
	ca_ni_internal_phy_init();
#endif

	start_time = get_timer(0);
	while (get_timer(start_time) < AUTO_SCAN_TIMEOUT) {
		for (phy_addr = 1; phy_addr < 5; phy_addr++) {
			ca_mdio_read(phy_addr, 1, &data);
			if (data & 0x04) {
				active_port = phy_addr - 1;
				printf("%s: active_port=%d\n", __func__, active_port);

				ca77xx_ni_led(active_port, 1);
				return;
			}
		}
	}
	printf("%s: auto scan active_port timeout, set active_port to 1.\n", __func__);
	active_port = NI_PORT_1;

	ca77xx_ni_led(active_port, 0);

}

#elif defined(CONFIG_TARGET_SATURN_ASIC)

/* auto scan the first link up port as active_port */
#define AUTO_SCAN_TIMEOUT       3000            /* 3 seconds */
static void ca77xx_ni_auto_scan_active_port(void)
{
	u8      phy_addr;
	u32     start_time;
	ca_uint16_t data;

	/* do internal GHPY reset */
	active_port = 3;
	cortina_ni_reset();

	/* should initialize internal GPHY, NI port 3 */
	phy_addr = 1;
	ca_mdio_write(phy_addr, 31, 0x0BC6);
	ca_mdio_write(phy_addr, 16, 0x0053);
	ca_mdio_write(phy_addr, 18, 0x4003);
	ca_mdio_write(phy_addr, 22, 0x7e01);
	ca_mdio_write(phy_addr, 31, 0x0A42);
	ca_mdio_write(phy_addr, 31, 0x0A40);
	ca_mdio_write(phy_addr,  0, 0x1140);

	/* workaround to fix GPHY fail */
	/* Clear clock fail interrupt */
	ca_mdio_write(phy_addr, 31, 0xB90);
	ca_mdio_read(phy_addr, 19, &data);
	//printf("%s: phy_addr=%d, read register 19, value=0x%x\n", __func__, phy_addr, data);
	if (data == 0x10) {
		ca_mdio_write(phy_addr, 31, 0xB90);
		ca_mdio_read(phy_addr, 19, &data);
		printf("%s: read again phy_addr=%d, read register 19, value=0x%x\n", __func__, phy_addr, data);
	}
#ifdef CORTINA_NI_DBG
	printf("%s: phy_addr=%d, read register 19, value=0x%x\n", __func__, phy_addr, data);
#endif

	start_time = get_timer(0);
	while (get_timer(start_time) < AUTO_SCAN_TIMEOUT) {
		phy_addr = 5;  /* NI port 4 */
		ca_mdio_read(phy_addr, 1, &data);
		if (data & 0x04) {
			active_port = NI_PORT_4;
			printf("%s: active_port=%d\n", __func__, active_port);
			return;
		}
		phy_addr = 1;  /* NI port 3 */
		ca_mdio_read(phy_addr, 1, &data);
		if (data & 0x04) {
			active_port = NI_PORT_3;
			printf("%s: active_port=%d\n", __func__, active_port);
			return;
		}
	}
	printf("%s: auto scan active_port timeout, set active_port to 3.\n", __func__);
	active_port = NI_PORT_3;
}

#endif

#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
#define GPHY_CAL_LEN	6
typedef struct gphy_cal_s {
	u32 reg_off;
	u32 value;
}gphy_cal_t;

static gphy_cal_t gphy_cal_vlaues[GPHY_CAL_LEN] = {
	{0xf43380fc, 0xbcd},
	{0xf43380dc, 0xeeee},
	{0xf43380d8, 0xeeee},
	{0xf43380fc, 0xbce},
	{0xf43380c0, 0x7777},
	{0xf43380c4, 0x7777}
};

static void do_internal_gphy_cal(void)
{
	int i, port;
	u32 reg_off, value;

	for (port = 0; port < 4; port++) {
		for (i = 0; i < GPHY_CAL_LEN; i++) {
			reg_off = gphy_cal_vlaues[i].reg_off + (port * 0x80);
			value = gphy_cal_vlaues[i].value;
			CORTINA_CA_REG_WRITE(value, reg_off);
			mdelay(50);
		}
	}
}
#endif

int ca77xx_eth_initialize(bd_t *bis)
{
	struct eth_device 	*dev;
	struct cortina_ni_priv 	*priv;
	char			*buf;
#if !defined(CONFIG_TARGET_VENUS) || !defined(CONFIG_TARGET_RTK_TAURUS) || !defined(CONFIG_TARGET_RTK_ELNATH) || !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
	u16			val;
#endif
	int		i;
	u16 		vendor_id, chip_id;
	u32		phy_id;
	struct mii_dev *mdiodev;
	int ret;
#ifdef CONFIG_REALTEK_10G_PON
	if(ca77xx_eth_initialize_done ==1){
		return 0;
	}
#endif
	dev = (struct eth_device *)malloc (sizeof *dev);
	if (NULL == dev)
		return 1;
	memset (dev, 0, sizeof *dev);

	priv = (struct cortina_ni_priv *)malloc (sizeof (*priv));
	if (NULL == priv)
		return 0;
	memset (priv, 0, sizeof *priv);

	priv->rx_xram_base_adr	= NI_XRAM_BASE + (RX_BASE_ADDR * 8);
	priv->rx_xram_end_adr	= NI_XRAM_BASE + ((RX_TOP_ADDR + 1) * 8);
	priv->rx_xram_start	= RX_BASE_ADDR;
	priv->rx_xram_end	= RX_TOP_ADDR;
	priv->tx_xram_base_adr	= NI_XRAM_BASE + (TX_BASE_ADDR * 8);
	priv->tx_xram_end_adr	= NI_XRAM_BASE + ((TX_TOP_ADDR + 1) * 8);
	priv->tx_xram_start	= TX_BASE_ADDR;
	priv->tx_xram_end	= TX_TOP_ADDR;

#if CORTINA_NI_DBG
	printf("%s: rx_base_addr:%x\t rx_top_addr %x\n", __func__, priv->rx_xram_start, priv->rx_xram_end);
	printf("%s: tx_base_addr:%x\t tx_top_addr %x\n", __func__, priv->tx_xram_start,priv->tx_xram_end);
	printf("%s: rx physical start address = %x end address = %x\n", __func__, priv->rx_xram_base_adr, priv->rx_xram_end_adr);
	printf("%s: tx physical start address = %x end address = %x\n",	__func__, priv->tx_xram_base_adr, priv->tx_xram_end_adr);
#endif

	//ni_enable_tx_rx();

	/* set MDIO pre-scale value */
	reg_value= CORTINA_CA_REG_READ(PER_MDIO_CFG);
	reg_value= reg_value | 0x00280000;
	CORTINA_CA_REG_WRITE(reg_value, PER_MDIO_CFG);

	/* In Saturn active_port are 3 or 4, because the register offset has been shifted forward */
	/* LAN ports (port 4-7) connect to RTL8214 */
#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
	if ((buf = env_get("auto_scan_active_port")) != 0) {
		auto_scan_active_port = simple_strtoul(buf, NULL, 0);
		printf("%s: auto_scan_active_port=%d\n", __func__, auto_scan_active_port);
	}

	if (auto_scan_active_port) {
		ca77xx_ni_auto_scan_active_port();
	}
	else {
	if ((buf = env_get("active_port")) != 0) {
			active_port = simple_strtoul(buf, NULL, 0);
		printf("%s: active_port=%d\n", __func__, active_port);
		if (active_port != NI_PORT_3 && active_port != NI_PORT_4) {
			printf("ERROR: does not support active_port %d!!\n", active_port);
			printf("Please change active_port to 3 or 4.\n");
			return 1;
		}
	}
	else {
		active_port = NI_PORT_4;
	}
	}
#else
	if ((buf = env_get("auto_scan_active_port")) != 0) {
		auto_scan_active_port = simple_strtoul(buf, NULL, 0);
		printf("%s: auto_scan_active_port=%d\n", __func__, auto_scan_active_port);
		if (auto_scan_active_port) {
			ca77xx_ni_auto_scan_active_port();
		}
		else {
			if ((buf = env_get("active_port")) != 0) {
				active_port = simple_strtoul(buf, NULL, 0);
				printf("%s: active_port=%d\n", __func__, active_port);
				if (active_port < NI_PORT_0 || active_port > NI_PORT_4) {
					printf("ERROR: does not support active_port %d!!\n", active_port);
					printf("Please change active_port to 0-3.\n");
					return 1;
				}
			}
			else {
				active_port = NI_PORT_1;
			}
		}
	}
	else {
		if ((buf = env_get("active_port")) != 0) {
			active_port = simple_strtoul(buf, NULL, 0);
			printf("%s: active_port=%d\n", __func__, active_port);
			if (active_port < NI_PORT_0 || active_port > NI_PORT_4) {
				printf("ERROR: does not support active_port %d!!\n", active_port);
				printf("Please change active_port to 0-3.\n");
				return 1;
			}
		}
		else {
			active_port = NI_PORT_1;
		}
		if (auto_scan_active_port) {
			ca77xx_ni_auto_scan_active_port();
		}
	}
#endif

	cortina_ni_reset();

#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
	/* port0: phy address 1 - GMAC0: port 0
	   port1: phy address 2 - GMAC1: port 1
	   port2: phy address 3 - GMAC2: port 2
	   port3: phy address 4 - GMAC3: port 3
	   port4: phy address 5 - RGMII: port 4
	*/
	ge_port_phy_addr = active_port + 1;
#else
	/* port0: phy address 1	- GMAC0: port 0
	   port1: phy address 2 - GMAC1: port 1
	   port2: phy address 3 - GMAC2: port 2
	   port3: phy address 4 - GMAC3: port 3
	   port4: phy address 16 - QSGMII: port 0
	   port5: phy address 17 - QSGMII: port 1
	   port6: phy address 18 - QSGMII: port 2
	   port7: phy address 19 - QSGMII: port 3
	*/
	if (active_port >= NI_PORT_0 && active_port <= NI_PORT_3) {
		ge_port_phy_addr = active_port + 1;
	}
	else {
		ge_port_phy_addr = active_port - 2;
	}
#endif

#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
       /* internal GPHY addr=1 */
       if (active_port == NI_PORT_3) {
               ge_port_phy_addr = 1;
       }
       else {
               ge_port_phy_addr = active_port + 1;
       }
	printf("%s: active_port=%d, ge_port_phy_addr=%d\n", __func__, active_port, ge_port_phy_addr);
#endif
	ca_mdio_read(ge_port_phy_addr, 2, &vendor_id);
	ca_mdio_read(ge_port_phy_addr, 3, &chip_id);
	phy_id = ((u32)vendor_id << 16) | chip_id;
#if defined(CORTINA_NI_DBG) || defined(CONFIG_RTK_V7_FPGA_PLATFORM)
	printf("%s: vendor_id=0x%x\n", __func__, vendor_id);
	printf("%s: chip_id=0x%x\n", __func__, chip_id);
	printf("%s: phy_id=0x%x\n", __func__, phy_id);
	printf("%s: phy_id & PHY_ID_MASK=0x%x\n", __func__, (phy_id & PHY_ID_MASK));
	printf("%s: PHY_ID_RTL8214=0x%x\n", __func__, PHY_ID_RTL8214);
#endif

	if ((phy_id & PHY_ID_MASK) == PHY_ID_RTL8211) {
#if !defined(CONFIG_TARGET_VENUS) || !defined(CONFIG_TARGET_RTK_TAURUS) || !defined(CONFIG_TARGET_RTK_ELNATH) || !defined(CONFIG_RTK_V7_FPGA_PLATFORM)

		printf("%s: do initial patch for PHY_ID_RTL8211\n", __func__);
		/*
		 * Disable response PHYAD=0 function of
		 * RTL8211 series PHY
		 */

		/* REG31 write 0x0007, set to extension page */
		ca_mdio_write(ge_port_phy_addr, 31, 0x0007);

		/* REG30 write 0x002C, set to extension page 44 */
		ca_mdio_write(ge_port_phy_addr, 30, 0x002C);

		/*
		 * REG27 write bit[2] =0
		 * disable response PHYAD=0  function.
		 * we should read REG27 and clear bit[2], and write back
		 */
		ca_mdio_read(ge_port_phy_addr, 27, &val);
		val &= ~(1<<2);
		ca_mdio_write(ge_port_phy_addr, 27, val);

		/* REG31 write 0X0000, back to page0 */
		ca_mdio_write(ge_port_phy_addr, 31, 0x0000);
#endif
	}

	/* the init sequency provided by RTK */
	if ((phy_id & PHY_ID_MASK) == PHY_ID_RTL8214) {
		printf("%s: write initial sequency for PHY_ID_RTL8214!!\n", __func__);
		for (i = 0; i < RTL8214_INIT_REG_COUNT; i++) {
			if (!(i & 1)) {
				//printf("[29, 0x%x]\n", rtl8214_init_reg_val[i]);
				ca_mdio_write(ge_port_phy_addr, 29, rtl8214_init_reg_val[i]);
			}
			else {
				//printf("[30, 0x%x]\n", rtl8214_init_reg_val[i]);
				ca_mdio_write(ge_port_phy_addr, 30, rtl8214_init_reg_val[i]);
			}
		}
	}

#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
    if ((phy_id & PHY_ID_MASK) == PHY_ID_RTL8211_G3_ASIC) {

		if (!auto_scan_active_port) {
			printf("%s: write initial sequency for PHY_ID_RTL8211_G3_ASIC!!\n", __func__);

			ca_ni_internal_phy_init();
			}
#if (defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
		else {
			ca77xx_ni_auto_scan_active_port();
		}
#endif
		}

	ca77xx_ni_scan_phy_link();
#endif

#if defined(CONFIG_RTK_V7_FPGA_PLATFORM)
        /* REG9 write 0x0200->0x0000, disable adv. 1000BASE capability */
        ca_mdio_write(ge_port_phy_addr, 0x09, 0x0000);
        /* REG0 write 0x1040->0x9040, reset and associate the PHY link
         * REG0 and REG1 will return to default values after reset */
        ca_mdio_read(ge_port_phy_addr, 0x00, &val);
        val |= 0x8000;
        ca_mdio_write(ge_port_phy_addr, 0x00, val);
        mdelay(1);

#endif

	/* parsing ethaddr and set to NI registers. */
	ni_setup_mac_addr();

	sprintf (dev->name, CONFIG_IDENT_STRING);
	dev->priv = priv;
	dev->init = cortina_ni_init;
	dev->halt = cortina_ni_halt;
	dev->send = cortina_ni_send;
	dev->recv = cortina_ni_recv;

	eth_register (dev);

	/* the phy_read and phy_write should meet the proto type of miiphy_register */
	#if 0
	miiphy_register(dev->name, ca_miiphy_read, ca_miiphy_write);
	#else
		/* the phy_read and phy_write should meet the proto type of miiphy_register */
	mdiodev = mdio_alloc();
	if (!mdiodev)
		return -ENOMEM;
	strncpy(mdiodev->name, dev->name, MDIO_NAME_LEN);
	mdiodev->read = ca_miiphy_read;
	mdiodev->write = ca_miiphy_write;
	ret = mdio_register(mdiodev);
	if (ret < 0)
		return ret;
	#endif



#if CORTINA_NI_DBG
	dbg_dev = dev;
#endif
	curr_dev = dev;

#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
	/* hardware settings for RGMII port */
	{
		GLOBAL_GLOBAL_CONFIG_t  glb_config;
		GLOBAL_IO_DRIVE_CONTROL_t io_drive_control;

		/* Generating 25Mhz reference clock for switch */
		glb_config.wrd = CORTINA_CA_REG_READ(GLOBAL_GLOBAL_CONFIG);
		glb_config.bf.refclk_sel = 0x01;
		glb_config.bf.ext_reset = 0x01;
		CORTINA_CA_REG_WRITE(glb_config.wrd, GLOBAL_GLOBAL_CONFIG);

		mdelay(20);

		/* should do a external reset */
		glb_config.wrd = CORTINA_CA_REG_READ(GLOBAL_GLOBAL_CONFIG);
		glb_config.bf.ext_reset = 0x0;
		CORTINA_CA_REG_WRITE(glb_config.wrd, GLOBAL_GLOBAL_CONFIG);

		io_drive_control.wrd = CORTINA_CA_REG_READ(GLOBAL_IO_DRIVE_CONTROL);
		io_drive_control.bf.gmac_mode = 2;
		io_drive_control.bf.gmac_dn = 1;
		io_drive_control.bf.gmac_dp = 1;
		CORTINA_CA_REG_WRITE(io_drive_control.wrd, GLOBAL_IO_DRIVE_CONTROL);
	}

	/* initialize LEDs for ethernet link and traffic lights */
	//ca77xx_init_led();

#if defined(CONFIG_TARGET_PRESIDIO_ASIC)
	/* do internal gphy calibration */
	do_internal_gphy_cal();
#endif

#endif
#ifdef CONFIG_REALTEK_10G_PON
	ca77xx_eth_initialize_done = 1;
#endif
	return 0;
}

#if (defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)
void ca77xx_eth_change_default(void)
{
	WRAP_GPHY_CTRL12_t wrap_gphy_ctrl12;
	WRAP_GPHY_CTRL13_t wrap_gphy_ctrl13;
	u8 phy_addr;
	u16 val;

	/* Set WRAP_GPHY_CTRL12 cfg_ocp_timeout to 0xff */
	wrap_gphy_ctrl12.wrd = CORTINA_CA_REG_READ(WRAP_GPHY_CTRL12);
	wrap_gphy_ctrl12.bf.cfg_ocp_timeout = 0xff;
	CORTINA_CA_REG_WRITE(wrap_gphy_ctrl12.wrd, WRAP_GPHY_CTRL12);

	/* Set WRAP_GPHY_CTRL13 patch_phy_done to 1 */
	wrap_gphy_ctrl13.wrd = CORTINA_CA_REG_READ(WRAP_GPHY_CTRL13);
	wrap_gphy_ctrl13.bf.patch_phy_done = 1;
	CORTINA_CA_REG_WRITE(wrap_gphy_ctrl13.wrd, WRAP_GPHY_CTRL13);

	for (phy_addr = 4; phy_addr > 0; phy_addr--) {
		/* REG31 write 0x0007, set to extension page */
		ca_mdio_write(phy_addr, 31, 0x0007);

		/* REG30 write 0x0A43, set to extension page 2627 */
		ca_mdio_write(phy_addr, 30, 0x0A43);

		/*
		 * REG24 write bit[7] =0
		 * fix half duplex unidirectional not wire speed.
		 * we should read REG24 and clear bit[7], and write back
		 */
		ca_mdio_read(phy_addr, 24, &val);
		val &= ~(1<<7);
		ca_mdio_write(phy_addr, 24, val);
	}
}
#endif

#if CORTINA_NI_DBG
DECLARE_GLOBAL_DATA_PTR;
int do_eth_init (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;
	cortina_ni_init(0x00,bd);
	return 0;
}

U_BOOT_CMD(
		do_eth_init,   2,      1,      do_eth_init,
		"do_eth_init\t- to test eth_init \n",
		"None\n"
	  );
#endif

#if CORTINA_NI_DBG
int do_eth_send (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char 	pkt[1536];
	unsigned int 	i;

	for (i=0; i<1500; i++){
		pkt[i] = i % 256;
	}

	for (i=60; i<360; i++) {
		cortina_ni_send(dbg_dev,pkt,i);
	}
	return 0;
}

U_BOOT_CMD(
		do_eth_send,   3,      2,      do_eth_send,
		"do_eth_send\t- to test eth_send \n",
		"None\n"
	  );

int do_eth_rx (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cortina_ni_recv(dbg_dev);
	return 0;
}

U_BOOT_CMD(
		do_eth_rx,   2,      1,      do_eth_rx,
		"do_eth_rx\t- to test eth_rx \n",
		"None\n"
	  );

int do_read_phy(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int	phy_adr;
	unsigned int	reg_off;
	ca_uint16_t	reg_val;

	phy_adr = simple_strtoul(argv[1], NULL, 0);
	reg_off = simple_strtoul(argv[2], NULL, 0);
	ca_mdio_read(phy_adr, reg_off, &reg_val);
	printf("PHY_ADR = %d offset=%d reg_val=%x \n",phy_adr,reg_off,reg_val);
	return 0;
}

U_BOOT_CMD(
		ca_phy_read,   3,      1,      do_read_phy,
		"do_read_phy\t- to read PHY register \n",
		"None\n"
	  );

int do_write_phy(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int	phy_adr;
	unsigned int	reg_off;
	unsigned int	reg_val;

	phy_adr = simple_strtoul(argv[1], NULL, 0);
	reg_off = simple_strtoul(argv[2], NULL, 0);
	reg_val = simple_strtoul(argv[3], NULL, 0);
	ca_mdio_write(phy_adr,reg_off,reg_val);
	printf("PHY_ADR = %d offset=%d reg_val=%x \n",phy_adr,reg_off,reg_val);
	return 0;
}

U_BOOT_CMD(
		ca_phy_write,   4,      1,      do_write_phy,
		"do_write_phy\t- to write PHY register \n",
		"None\n"
	  );

#endif

static int do_phy_reg(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret, i;
	u16 phy_addr, reg, val;

	if (argc < 2) {
		printf("Usage:\nphy_reg_value%s\n", cmdtp->help);
		return -1;
	}

	phy_addr = simple_strtoul(argv[1], NULL, 10);

	if (phy_addr > 31) {
		printf("Usage:\nphy_reg_value%s\n", cmdtp->help);
		return -1;
	}
	if (argc == 2) {
		/* read the first 15 registers of the PHY */
		printf("PHY addr %d:\n", phy_addr);
		for (i = 0; i < 15; i++) {
			ca_mdio_read(phy_addr, i, &val);
			printf("Reg 0x%04X = 0x%04X\n", i, val);
		}
		return 0;
	}

	reg = simple_strtoul(argv[2], NULL, 10);

	if (argc == 3) {
		/* read cmd */
		ca_mdio_read(phy_addr, reg, &val);
		printf("PHY addr %d Reg 0x%04X = 0x%04X\n", phy_addr, reg, val);
	} else {
		/* argc > 3*/
		/* write cmd */
		val = simple_strtoul(argv[3], NULL, 10);
		ret = ca_mdio_write(phy_addr, reg, val);
		if (!ret) {
			printf("PHY addr %d Reg 0x%04X = 0x%04X\n", phy_addr, reg, val);
		} else {
			printf("Can't write PHY addr %d Reg 0x%04X as 0x%04X, ret = %d\n", phy_addr, reg, val, ret);
		}
	}
	return 0;
}

U_BOOT_CMD(
		phy_reg,	4,	1,	do_phy_reg,
		"read/write PHY register",
		"[PHY addr] [reg_valueaddr] ([value])\n"
		"PHY addr : 0-31\n"
	  );

#ifdef CONFIG_MK_CUSTOMB
/* code custom switch register access function here */
#endif

#ifdef CONFIG_MK_REFERENCEQ
int do_qca_reg(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret, i;
	unsigned int reg, val, len;

	if (argc < 2) {
		printf("Usage:\nqca_reg_value%s\n", cmdtp->help);
		return -1;
	}

	reg_value= simple_strtoul(argv[1], NULL, 10);

	if (argc == 2) {
		/* read the first 15 registers of the PHY */
		printf("REG addr 0x%x:\n", reg_value);
		for (i = 0; i < 1; i++) {
			isis_reg_get(0, reg+i*4, (uint8_t *)&val, sizeof(uint32_t));
			printf("Reg 0x%04X = 0x%04X\n", reg+i*4, val);
		}
		return 0;
	}

	if (argc == 3) {
		/* write cmd */
		val = simple_strtoul(argv[2], NULL, 10);
		ret = isis_reg_set(0, reg, (uint8_t *)&val, sizeof(uint32_t));
		if (!ret) {
			printf("Reg 0x%04X = 0x%04X\n", reg, val);
		} else {
			printf("Can't write %d Reg 0x%04X as 0x%04X, ret = %d", reg, val, ret);
		}
	}
	return 0;
}

U_BOOT_CMD(
		qca_reg,	3,	1,	do_qca_reg,
		"read/write switch internal registers",
		"[reg_valueoffset] ([value]) \n"
	  );
#endif
