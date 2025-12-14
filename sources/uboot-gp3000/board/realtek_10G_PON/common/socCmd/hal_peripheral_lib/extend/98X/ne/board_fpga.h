#ifndef __BOARD_FPGA_H__
#define __BOARD_FPGA_H__

#include "ca_ni.h"
#if 0//yocto
#else//sd1 uboot for 98f
#include <linux/ethtool.h>
#include <linux/mii.h>
#endif

u8 ca_ni_eth_mac[CA_NI_MAX_INTF][6] = {
	{0x00, 0x13, 0x25, 0x00, 0x00, 0x01},
	{0x00, 0x13, 0x25, 0x00, 0x00, 0x02} };

/* pre-defined NI port config */
//static ca_port_cfg_info_t port_cfg_infos[CA_NI_MAX_INTF] = {
ca_port_cfg_info_t port_cfg_infos[CA_NI_MAX_INTF] = {
	[0] = { 					/* port 4 as WAN port (eth0) */
		.duplex = DUPLEX_FULL,
		.flowctrl = FLOW_CTRL_TX | FLOW_CTRL_RX,
		.num_phy_port = 1,
		.port_id = {CA_NI_GE0_PORT},
		.rx_ldpid = CA_NI_WAN_L2RP0_LPID,
		.tx_ldpid = CA_NI_GE7_PORT,
		.irq = 33,				/* should get the correct from device tree */
		.mac_addr = ca_ni_eth_mac[0],
		.rmii_clk_src = 0,
	},
	[1] = { 					/* port 0-3 as LAN ports (eth1) */
		.duplex = DUPLEX_FULL,
		.flowctrl = FLOW_CTRL_TX | FLOW_CTRL_RX,
		.num_phy_port = 3,
		.port_id = {CA_NI_PORT1, CA_NI_PORT2, CA_NI_PORT3},
		.rx_ldpid = CA_NI_LAN_L2RP0_LPID,
		.tx_ldpid = CA_NI_L2_L3_LAN_LPID,	/* go through L2FE DMA LSO should not turn on bypass L2FE */
		.irq = 41,				/* should get the correct from device tree */
		.mac_addr = ca_ni_eth_mac[1],
		.rmii_clk_src = 0,
	},
};

#endif /* __BOARD_FPGA_H__ */
