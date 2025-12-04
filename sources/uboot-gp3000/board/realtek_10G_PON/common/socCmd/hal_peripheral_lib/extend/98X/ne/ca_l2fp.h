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
#ifndef __CA_L2FP_H__
#define __CA_L2FP_H__

#include "osal_pkt.h"

#define CA_L2FP_DEVICE_CAP_DROP_UNKNOW_VLAN		0x1
#define CA_L2FP_DEVICE_CAP_MEMBERSHIP_CHECK		0x2
#define CA_L2FP_DEVICE_CAP_INGRESS_VLAN_ACTION	0x4
#define CA_L2FP_DEVICE_CAP_EGRESS_VLAN_ACTION	0x8

#define CA_L2FP_DEVICE_CAP_MAX					0x3

#define CA_L2FP_FLOOD_LDPID						0xFF

typedef ca_status_t (*cb_fn_rx_802_3)(
	void *priv_data,
	ca_uint8 cos,
	pkt_t * skb);

typedef ca_status_t (*cb_fn_xmit_done)(
	void *priv_data,
	pkt_t * skb);

typedef struct {
	void *				priv_data; 		/* device private data pointer*/
	cb_fn_rx_802_3		rx_802_3_fn;	/*
										 * If device return CA_E_OK, the device
										 * need to free the pkt by itself
										 * or the host will free the pkt
										 */
	cb_fn_xmit_done		xmit_done_fn;
	ca_uint32_t			capacity; 		/* device capacity*/
} ca_l2fp_device_entry_t;


ca_status_t ca_l2fp_receive_pkt(
	CA_IN  ca_uint32_t cpu_idx,
	CA_IN  ca_uint32_t voq,
	CA_IN  pkt_t * pkt);

ca_status_t ca_l2fp_device_register (
	CA_IN		ca_l2fp_device_entry_t *	p_l2fp_device_entry,
	CA_OUT		ca_uint8_t *				p_dev_id);

ca_status_t ca_l2fp_device_unregister (
	CA_IN		ca_uint8_t 				device_id);

ca_status_t ca_l2fp_device_pkt_xmit (
	CA_IN		ca_uint8_t 				device_id,
	CA_IN 		ca_uint8_t 					priority_queue,
	CA_IN 		ca_uint8_t					dest_port,
	CA_IN		pkt_t *						pkt);

ca_status_t ca_l2fp_cpu_port_id_get (
	CA_IN		ca_uint8_t 				device_id,
	CA_OUT		ca_uint32_t *				p_cpu_id);

ca_status_t ca_l2fp_device_dump (void);
ca_status_t ca_l2fp_init(void);
ca_status_t ca_l2fp_cleanup(void);


#endif /* __CA_L2FP_H__ */

