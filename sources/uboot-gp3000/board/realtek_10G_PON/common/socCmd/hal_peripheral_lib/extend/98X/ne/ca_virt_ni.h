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

#ifndef __CA_VIRT_NI_H__
#define __CA_VIRT_NI_H__

#if 0//yocto
#include <linux/if_ether.h>
#else//sd1 uboot - TBD
#endif

//#define ETH_P_8021AD		0x88A8

typedef struct ca_virt_ni_s {
	struct net_device *dev;
	struct net_device *real_dev;

#define NI_ACTIVE	1
#define NI_INACTIVE	0
	u8 flag;
} ca_virt_ni_t;

/* use flag CONFIG_CS752X_VIRTUAL_ETH0/1/2 to determine whether or not
 * to create virtual interface on eth0/1/2 */
/* use flag CONFIG_CS752X_NR_VIRTUAL_ETH[0/1/2] to determine how many
 * interfaces are needed to create under that physical interface */

///TODO: only support one currently???
#define NR_VIRT_NI_ETH0	1
#define VID_START_ETH0	1

#define NR_VIRT_NI_ETH1	1
#define VID_START_ETH1	1

#define NR_VIRT_NI_ETH2	1
#define VID_START_ETH2	1

ca_virt_ni_t *ca_ni_get_virt_ni(u8 vpid, struct net_device *virt_dev);
int ca_ni_virt_ni_create_if(int port_id, struct net_device *real_dev);
int ca_ni_virt_ni_open(u8 port_id, struct net_device *dev);
int ca_ni_virt_ni_close(struct net_device *dev);
void ca_ni_virt_ni_remove_if(u8 port_id);
int ca_ni_virt_ni_process_rx_skb(struct sk_buff *skb);
int ca_ni_virt_ni_process_tx_skb(struct sk_buff *skb, struct net_device *dev, struct netdev_queue *txq);

#endif	/* __CA_VIRT_NI_H__ */

