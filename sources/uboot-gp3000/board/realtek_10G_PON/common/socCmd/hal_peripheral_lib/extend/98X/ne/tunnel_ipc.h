#ifndef __TUNNEL_IPC_H__
#define __TUNNEL_IPC_H__

#include "ca_types.h"
#include "tunnel.h"

/***************************************************************************
 * IPC functions - Generic
 ***************************************************************************/

typedef struct {
	ca_tunnel_type_t	tunnel_type;
	ca_tunnel_id_t		tunnel_id;
	ca_uint16_t		mtu;
} ca_tunnel_mtu_t;

ca_status_t ca_tunnel_mtu_set(
	CA_IN	ca_device_id_t	device_id,
	CA_IN	ca_tunnel_mtu_t	*p_mtu);

ca_status_t ca_tunnel_mtu_get(
	CA_IN	ca_device_id_t	device_id,
	CA_OUT	ca_tunnel_mtu_t	*p_mtu);

/***************************************************************************
 * IPC functions - L2TP
 ***************************************************************************/

/***************************************************************************
 * IPC functions - IPSec
 ***************************************************************************/

ca_status_t ca_ipsec_ipc_sadb_set(
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_tunnel_direction_t	dir,
	CA_IN	ca_tunnel_cfg_t		*tunnel_cfg,
	CA_IN	ca_ipsec_sa_id_t	sa_id);

ca_status_t ca_ipsec_ipc_sadb_del(
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_tunnel_direction_t	dir,
	CA_IN	ca_ipsec_sa_id_t	sa_id);

/***************************************************************************
 * IPC initialization
 ***************************************************************************/

ca_status_t ca_tunnel_ipc_init(/*CODEGEN_IGNORE_TAG*/ ca_device_id_t device_id);
ca_status_t ca_tunnel_ipc_exit(/*CODEGEN_IGNORE_TAG*/ ca_device_id_t device_id);

#endif /* __TUNNEL_IPC_H__ */

