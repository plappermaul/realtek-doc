#ifndef __TUNNEL_IPC_INTERNAL_H__
#define __TUNNEL_IPC_INTERNAL_H__

#include "ca_types.h"
#include "tunnel.h"

/***************************************************************
 * CA_IPC_PE_MTU
 ***************************************************************/

typedef struct {
	ca_tunnel_type_t	tunnel_type;
	ca_uint32_t		sa_id;
	ca_uint16_t		mtu;
} __attribute__((packed)) ca_ipc_pe_mtu_set_t;

typedef struct {
	ca_tunnel_type_t	tunnel_type;
	ca_uint32_t		sa_id;
} __attribute__((packed)) ca_ipc_pe_mtu_get_t;

typedef struct {
	ca_tunnel_type_t	tunnel_type;
	ca_uint32_t		sa_id;
	ca_uint16_t		mtu;
} __attribute__((packed)) ca_ipc_pe_mtu_get_ack_t;

/***************************************************************
 * CA_IPSEC_IPC_PE
 ***************************************************************/

typedef struct {
	ca_ipsec_sa_id_t	sa_id;
	ca_ipsec_sa_t		ipsec_sa;

	ca_l3_ip_addr_t		tunnel_saddr;
	ca_l3_ip_addr_t		tunnel_daddr;

	ca_uint32_t		lifetime_bytes;
	ca_uint32_t		bytes_count;
	ca_uint32_t		lifetime_packets;
	ca_uint32_t		packets_count;

	ca_uint16_t		ip_checksum;
	ca_uint8_t		copy_ip_id;
	ca_uint8_t		copy_tos;
	ca_uint8_t		default_tos; /* Set the default dscp value */
} __attribute__((__packed__)) ca_ipsec_ipc_sadb_set_t;

typedef struct {
	ca_uint32_t		ret_value;
} __attribute__((__packed__)) ca_ipsec_ipc_sadb_set_ack_t;

typedef struct {
        ca_ipsec_sa_id_t	sa_id;
} __attribute__((__packed__)) ca_ipsec_ipc_sadb_del_t;

typedef struct {
	ca_ipsec_sa_id_t	sa_id;
	ca_uint16_t		offload_seq;
} __attribute__((__packed__)) ca_ipsec_ipc_sadb_del_ack_t;

/***************************************************************
 * CA_L2TP_IPC_PE
 ***************************************************************/

typedef struct {
	ca_l2tp_tunnel_cfg_t	l2tp;

	/* For L2TP over IPSec */
	ca_ipsec_sa_id_t	sa_id;
	ca_uint16_t		ip_ver;
	ca_uint16_t		ip_checksum;
	ca_l3_ip_addr_t		tunnel_saddr;
	ca_l3_ip_addr_t		tunnel_daddr;
} __attribute__((__packed__)) ca_l2tp_ipc_entry_set_t;

typedef struct {
	ca_uint32_t		ret_value;
} __attribute__((__packed__)) ca_l2tp_ipc_entry_set_ack_t;

typedef struct {
	ca_uint16_t     sa_id;
} __attribute__((__packed__)) ca_l2tp_ipc_entry_del_t;


typedef struct {
	ca_uint16_t	sa_id;
	ca_uint16_t	offload_seq;
} __attribute__((__packed__)) ca_l2tp_ipc_entry_del_ack_t;


#endif /* __TUNNEL_IPC_INTERNAL_H__ */

