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
#ifndef __CA_PHY_H__
#define __CA_PHY_H__

#if 0//yocto
#include <linux/netdevice.h>
#else//sd1 uboot for 98f
#endif
#include "ca_ni.h"

#define PHY_ID_RTL8211C			0x001cc910
#define PHY_ID_RTL8201E			0x001cc815
#define PHY_ID_RTL8201F			0x001cc816
#define PHY_ID_RTL8211E			0x001cc915
#define PHY_ID_RTL8214C			0x001cca60
#define PHY_ID_ATH8021			0x004dd04e
#define PHY_ID_ATH8032			0x004dd023
#define PHY_ID_QCA8337			0x004dd035 /* internal PHY of QCA8337(N) */

#define MAC_PHYCFG2_EMODE_MASK_RT8211	0x00000000
#define MAC_PHYCFG2_EMODE_MASK_RT8201	0x000001c0
#define MAC_PHYCFG2_EMODE_COMP_RT8211	0x00000800
#define MAC_PHYCFG2_EMODE_COMP_RT8201	0x00000000
#define MAC_PHYCFG2_FMODE_MASK_RT8211	0x00000000
#define MAC_PHYCFG2_FMODE_MASK_RT8201	0x00007000
#define MAC_PHYCFG2_GMODE_MASK_RT8211	0x00000000
#define MAC_PHYCFG2_GMODE_MASK_RT8201	0x001c0000
#define MAC_PHYCFG2_GMODE_COMP_RT8211	0x00200000
#define MAC_PHYCFG2_GMODE_COMP_RT8201	0x00000000
#define MAC_PHYCFG2_ACT_MASK_RT8211	0x03000000
#define MAC_PHYCFG2_ACT_MASK_RT8201	0x01000000
#define MAC_PHYCFG2_ACT_COMP_RT8211	0x00000000
#define MAC_PHYCFG2_ACT_COMP_RT8201	0x08000000
#define MAC_PHYCFG2_QUAL_MASK_RT8211	0x30000000
#define MAC_PHYCFG2_QUAL_MASK_RT8201	0x30000000
#define MAC_PHYCFG2_QUAL_COMP_RT8211	0x00000000
#define MAC_PHYCFG2_QUAL_COMP_RT8201	0x00000000
#define MAC_PHYCFG2_FMODE_COMP_RT8211	0x00038000
#define MAC_PHYCFG2_FMODE_COMP_RT8201	0x00000000

#define MAC_PHYCFG2_RTL8211C_LED_MODES \
	(MAC_PHYCFG2_EMODE_MASK_RT8211 | \
	 MAC_PHYCFG2_EMODE_COMP_RT8211 | \
	 MAC_PHYCFG2_FMODE_MASK_RT8211 | \
	 MAC_PHYCFG2_FMODE_COMP_RT8211 | \
	 MAC_PHYCFG2_GMODE_MASK_RT8211 | \
	 MAC_PHYCFG2_GMODE_COMP_RT8211 | \
	 MAC_PHYCFG2_ACT_MASK_RT8211 | \
	 MAC_PHYCFG2_ACT_COMP_RT8211 | \
	 MAC_PHYCFG2_QUAL_MASK_RT8211 | \
	 MAC_PHYCFG2_QUAL_COMP_RT8211)
#define MAC_PHYCFG2_RTL8201E_LED_MODES \
	(MAC_PHYCFG2_EMODE_MASK_RT8201 | \
	 MAC_PHYCFG2_EMODE_COMP_RT8201 | \
	 MAC_PHYCFG2_FMODE_MASK_RT8201 | \
	 MAC_PHYCFG2_FMODE_COMP_RT8201 | \
	 MAC_PHYCFG2_GMODE_MASK_RT8201 | \
	 MAC_PHYCFG2_GMODE_COMP_RT8201 | \
	 MAC_PHYCFG2_ACT_MASK_RT8201 | \
	 MAC_PHYCFG2_ACT_COMP_RT8201 | \
	 MAC_PHYCFG2_QUAL_MASK_RT8201 | \
	 MAC_PHYCFG2_QUAL_COMP_RT8201)

#if 0//yocto
int ca_mdio_init(ca_eth_private_t *gep);
int ca_mdio_exit(ca_eth_private_t *gep);

void ca_phy_adjust_link(struct net_device *dev);
int ca_phy_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
int ca_phy_init(ca_eth_private_t *gep, int phy_addr);
int ca_phy_exit(ca_eth_private_t *gep);
#else//sd1 uboot for 98f
#endif

#endif /* __CA_PHY_H__ */

