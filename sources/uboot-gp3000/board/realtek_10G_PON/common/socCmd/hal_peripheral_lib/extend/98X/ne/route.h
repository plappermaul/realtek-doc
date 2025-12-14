#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "ca_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CA_L3_IP_MAX_INTF		64	/* maximal IP interface number */
#define CA_IF_ID_INVALID		0xffffffff

#define CA_6RD_INTF_MIN			32	/* minimal interface ID for 6RD */
#define CA_6RD_INTF_MAX			47	/* maximal interface ID for 6RD */

#define CA_DSLITE_INTF_MIN		48	/* minimal interface ID for DSLite */
#define CA_DSLITE_INTF_MAX		63	/* maximal interface ID for DSLite */

#define CA_L3_IPV4_MAX_ROUTE		16	/* maximal IPv4 route number */
#define CA_L3_IPV6_MAX_ROUTE		16	/* maximal IPv6 route number */

#define CA_L3_IPV4_MAX_RT_NEXTHOP	16	/* maximal IPv4 route nexthop number */
#define CA_L3_IPV6_MAX_RT_NEXTHOP	16	/* maximal IPv6 route nexthop number */

#define CA_L3_MAX_NEIGH_NEXTHOP		4096	/* maximal neighbour nexthop number */

#define CA_L3_MAX_ROUTE_NEXTHOP		(CA_L3_IPV4_MAX_RT_NEXTHOP + CA_L3_IPV6_MAX_RT_NEXTHOP)
#define CA_L3_MAX_NEXTHOP_NUM		(CA_L3_MAX_ROUTE_NEXTHOP + CA_L3_MAX_NEIGH_NEXTHOP)

#define CA_L3_NEXTHOP_AGING_TIME_MAX	3600	/* maximal value of aging time */
#define CA_L3_NEXTHOP_AGING_TIME	300	/* default aging time: 300s */

#define CA_L3_NEXTHOP_ID_OVERFLOW_FLAG	0x800	/* indicate the entry is in overflow table */
#define CA_L3_NEXTHOP_HASH_IDX_MASK	0x7FF	/* mask for hash index for hash table and overflow table */

typedef enum {
	CA_L3_INTF_BCAST	= 0x00000001,	/* broadcast interface */
	CA_L3_INTF_P2P		= 0x00000002,	/* P2P interface  */
	CA_L3_INTF_TUNNEL	= 0x00000003,	/* tunnel interface (PPPoE is of type tunnel) */
	CA_L3_INTF_CPU		= 0x00000004,	/* an interface associated with CPU port to direct local packets */
	CA_L3_INTF_LB		= 0x00000005,
} ca_l3_intf_type_t;

typedef struct ca_l3_intf_stat_s {
	ca_uint64_t      tx_csum_err;
	ca_uint64_t      rx_csum_err;
	ca_uint64_t      tx_bytes;
	ca_uint64_t      rx_bytes;
	ca_uint64_t      tx_pkts;
	ca_uint64_t      rx_pkts;
	ca_uint64_t      tx_drop_bytes;
	ca_uint64_t      rx_drop_bytes;
	ca_uint64_t      tx_drop_pkts;
	ca_uint64_t      rx_drop_pkts;
} ca_l3_intf_stat_t;

typedef struct ca_l3_intf_s {
	ca_l3_intf_type_t	type;			/* L3 Intface type */
	ca_intf_id_t		intf_id;		/* IP Interface ID */
	ca_port_id_t		port_id;		/* PHY port intf is bound to */
	ca_mac_addr_t		mac_addr;		/* SRC MAC address to use on Intf */
	ca_uint16_t		tpid;			/* Outer TPID */
	ca_uint16_t		vid;			/* Outer VLAN */
	ca_tunnel_id_t		tunnel_id;		/* Tunnel Id */
	ca_uint16_t		mtu;			/* L3 MTU */
	ca_ip_address_t		ip_addr;		/* IP address of Intface */
	ca_uint8_t		nat_enable;		/* NAT enabled/disable */
} ca_l3_intf_t;

typedef enum {
	CA_L3_NHOP_ATTR_NEGH	= 0x00000000,		/* neighbor nexthop; aging */
	CA_L3_NHOP_ATTR_ROUTER	= 0x00000001,		/* router nexthop; never aging */
} ca_l3_nexthop_attr_t;

typedef struct ca_l3_nexthop_s {
	ca_l3_nexthop_id_t	nh_id;			/* 16-bit number: uniquely identify next hop entry */
	ca_l3_nexthop_attr_t	attr_flags;		/* Nexthop attribute flags */
	ca_ip_address_t		addr;			/* Nexthop IP address */
	ca_intf_id_t		intf_id;		/* Egress interface index */
	ca_mac_addr_t		da_mac;			/* Mac Address to be used for DA replacement */
	ca_uint32_t		aging_timer;
	/* For entry get only:
	   This field indicates the current hardware aging
	   timer value of the entry. (unit in seconds)
	   Global next hop entry aging timeout is set via
	   ca_l3_nexthop_aging_timer_set
	 */
} ca_l3_nexthop_t;

typedef struct ca_l3_route_s {
	ca_ip_address_t		prefix;		/* Matching key: LPM prefix */
	ca_l3_nexthop_id_t	nh_id;		/* Matching result: Nexthop index */
} ca_l3_route_t;

#define __INTF_STAT_NUM		18	/* L3 statistic fields number */

ca_status_t ca_l3_intf_add (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_l3_intf_t		*intf);

ca_status_t ca_l3_intf_update (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_intf_t		*intf);

ca_status_t ca_l3_intf_delete (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_intf_t		*intf);

ca_status_t ca_l3_intf_get(
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_l3_intf_t		*intf);

ca_status_t ca_l3_intf_iterate (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_iterator_t		*p_return_entry);

ca_status_t ca_l3_intf_stat_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_intf_id_t		intf_id,
	CA_IN		ca_boolean		read_keep,
	CA_OUT		ca_l3_intf_stat_t	*stat);

ca_status_t ca_l3_intf_stat_clear (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_intf_id_t		intf_id);

ca_status_t ca_l3_intf_stat_clear_all (
	CA_IN		ca_device_id_t		device_id);

ca_status_t ca_l3_route_add (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_route_t		*route);

ca_status_t ca_l3_route_delete (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_route_t		*route);

ca_status_t ca_l3_route_get (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_l3_route_t		*route);

ca_status_t ca_l3_route_iterate (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_ip_afi_t		afi,
	CA_OUT		ca_iterator_t		*p_return_entry );

ca_status_t ca_l3_nexthop_add (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_nexthop_t		*nexthop,
	CA_OUT		ca_l3_nexthop_id_t	*nh_id);

ca_status_t ca_l3_nexthop_update (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_nexthop_t		*nexthop,
	CA_IN		ca_l3_nexthop_id_t	nh_id);

ca_status_t ca_l3_nexthop_delete (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_nexthop_id_t	nh_id);

ca_status_t ca_l3_nexthop_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_l3_nexthop_id_t	nh_id,
	CA_OUT		ca_l3_nexthop_t		*nexthop);

ca_status_t ca_l3_nexthop_get_by_ip (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_ip_address_t		nh_ip,
	CA_OUT		ca_l3_nexthop_t		*nexthop);

ca_status_t ca_l3_nexthop_aging_timer_set (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_uint32_t		time);

ca_status_t ca_l3_nexthop_aging_timer_get (
	CA_IN		ca_device_id_t		device_id,
	CA_OUT		ca_uint32_t		*time);

ca_status_t ca_l3_nexthop_iterate (
	CA_IN		ca_device_id_t		device_id,
	CA_IN		ca_ip_afi_t		afi,
	CA_OUT		ca_iterator_t		*p_return_entry );

/***************************************************************************
 * Internal Use
 ***************************************************************************/

struct ca_int_l3_intf_s {/*CODEGEN_IGNORE_TAG*/
	ca_l3_intf_t intf;
	ca_uint16 cls_profile;
	ca_uint16 cls_idx;
	ca_uint8 use;
	ca_uint8 mtu_idx;
	ca_uint8 mac_idx;
	ca_uint32 hashlite_idx; /* host route to CPU port */
	ca_uint8 hashlite_idx_valid;
	ca_uint32 lpm_idx; /* LPM entry of MyIP */
	ca_uint8 lpm_idx_valid;
};

#include "classifier.h"
#define /*CODEGEN_IGNORE_TAG*/ CA_CLASS_INTF_CUSOMIZE		0x000000FF

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint32_t		port_id			:1;
	ca_uint32_t		sw_id			:1;
	ca_uint32_t		mtu 			:1;
	ca_uint32_t		src_mac 		:1;
	ca_uint32_t		dst_mac 		:1;
	ca_uint32_t		outer_vid		:1;
	ca_uint32_t		inner_vid		:1;
	ca_uint32_t 	outer_tpid		:1;
	ca_uint32_t 	inner_tpid		:1;
	ca_uint32_t 	outer_dei		:1;
	ca_uint32_t 	inner_dei		:1;
	ca_uint32_t 	outer_pbit		:1;
	ca_uint32_t 	inner_pbit		:1;
	ca_uint32_t 	nat_enable		:1;
	ca_uint32_t		egress_pppoe_action 	:1;
} ca_gen_intf_action_mask_t;


typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_intf_id_t		        intf_id;
	/*the keys for generic interface*/
	ca_classifier_key_t      key;
	ca_classifier_key_mask_t key_mask;
	ca_uint32_t 			 key_sw_id;	/* sw_id */
	ca_uint16_t 			 key_pppoe_session_id; /* valid if pppoe_action is set
													*  0: pop pppoe_session
													*  x: encapulate pppose_session
													*/

	/*the action as egress intf*/
	ca_uint8_t			prio;	/* resolution prio 0~7 in case of multiple hits */

	ca_gen_intf_action_mask_t	action_mask;
	ca_uint32_t 		sw_id;  /* sw_id */
	ca_uint32_t 		mtu;    /* max size */
	//ca_flow_mtu_action_t 	mtu_action;
	ca_uint8_t 			nat_enable;
} ca_gen_intf_attrib_t;



ca_status_t ca_generic_intf_add(/*CODEGEN_IGNORE_TAG*/
	CA_IN  ca_device_id_t 			device_id,
	CA_IN  ca_gen_intf_attrib_t		*gen_intf_attrib);

ca_status_t ca_generic_intf_del(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t 			device_id,
	CA_IN  ca_intf_id_t		        intf_id);

#ifdef __cplusplus
}
#endif

#endif /* __ROUTE_H__ */

