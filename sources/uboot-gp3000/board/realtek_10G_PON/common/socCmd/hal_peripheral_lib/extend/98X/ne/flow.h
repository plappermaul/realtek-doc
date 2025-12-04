#ifndef __FLOW_H__
#define __FLOW_H__

#include "ca_types.h"
#include "classifier.h"

/***************************************************************************
 * Flow Keys
 ***************************************************************************/

typedef enum {
	CA_FLOW_TYPE_0,		/* Customer Defined key set#0 */
	CA_FLOW_TYPE_1,		/* Customer Defined key set#1 */
	CA_FLOW_TYPE_2,		/* Customer Defined key set#2 */
	CA_FLOW_TYPE_3,		/* Customer Defined key set#3 */
	CA_FLOW_TYPE_4,		/* Customer Defined key set#4 */
	CA_FLOW_TYPE_5,		/* Customer Defined key set#5 */
	CA_FLOW_TYPE_6,		/* Customer Defined key set#6 */
	CA_FLOW_TYPE_7,		/* Customer Defined key set#7 */

	CA_FLOW_TYPE_MAX,
} ca_flow_key_type_t;

typedef struct {
	ca_uint32_t			o_lspid		:1;
	ca_uint32_t			lspid		:1;
	ca_uint32_t			src_mac		:1;
	ca_uint32_t			dest_mac	:1;
	ca_classifier_vlan_mask_t	outer_vlan;
	ca_classifier_vlan_mask_t	inner_vlan;
} ca_flow_key_l2_mask_t;

typedef struct {
	ca_port_id_t			o_lspid;
	ca_port_id_t			lspid;
	ca_mac_addr_t			src_mac;	/* source MAC address */
	ca_mac_addr_t			dest_mac;	/* dest MAC address */
	ca_classifier_vlan_t		outer_vlan;	/* ingress outer vlan */
	ca_classifier_vlan_t		inner_vlan;	/* ingress inner vlan */
} ca_flow_key_l2_t;

typedef struct {
	ca_uint32_t			ip_valid	:1;
	ca_ip_afi_t			ip_version	:1;
	ca_uint32_t			ip_protocol	:1;
	ca_uint32_t			dscp		:1;
	ca_uint32_t			ecn		:1;
	ca_uint32_t			ip_sa		:1;
	ca_uint32_t			ip_da		:1;
	ca_uint32_t			fragment	:1;
	ca_uint32_t			icmp_type	:1;
	ca_uint32_t			igmp_type	:1;
	ca_uint32_t			hbh		:1;	/* hbh header present */
	ca_uint32_t			routing_hdr	:1;	/* routing header present */
	ca_uint32_t			dest_opt_hdr	:1;	/* dest opt header present */
	ca_uint8_t			ip_ttl;			/* Support range match or exact match. 0xFF: exact match */
} ca_flow_key_l3_mask_t;

typedef struct {
	ca_boolean_t			ip_valid;	/* valid IP check */
	ca_uint32_t			ip_version;	/* IP version, V4 = 4 or V6 = 6 */
	ca_uint8_t			ip_protocol;	/* IP protocol, IPv6 in next-header*/
	ca_uint8_t			dscp;		/* DSCP */
	ca_uint8_t			ecn;		/* ECN */
	ca_ip_address_t			ip_sa;		/* ip_sa source IP address */
	ca_ip_address_t			ip_da;		/* ip_da destination IP address */
	ca_uint8_t			ip_ttl;		/* When l3_mask.ip_ttl != 0xFF
							   hash tuple will be considered matched when
							   ip_ttl_mask & rx_pkt.ip_ttl == flow.ip_ttl */
	ca_uint8_t			fragment;	/* fragment status :
							   0¡Vno MF and offset = 0,
							   1¡VMF or offset != 0 */
	ca_uint8_t			icmp_type;	/* ICMP message type */
	ca_uint8_t			igmp_type;	/* IGMP message type */
	ca_uint8_t			hbh;		/* hbh header present */
	ca_uint8_t			routing_hdr;	/* routing header present */
	ca_uint8_t			dest_opt_hdr;	/* dest opt header present */
} ca_flow_key_l3_t;

typedef struct {
	ca_uint32_t			src_port	:1;
	ca_uint32_t			dst_port	:1;
	ca_uint16_t			tcp_flags	:9;	/* {N,C,E,U,A,P,R,S,F} */
} ca_flow_key_l4_mask_t;

typedef struct {
	ca_uint16_t			src_port;	/* source port range, start = end for one */
	ca_uint16_t			dst_port;	/* destination port range, start = end for one */
	ca_uint16_t			tcp_flags;	/* Hash tuple will be considered matched when
							   rx_pkt.tcp_flags & l4_mask.tcp_flags == flow.tcp_flags */
} ca_flow_key_l4_t;

typedef struct {
	ca_uint32_t			sw_id		:1;
	ca_uint32_t			flow_id		:1;
	ca_uint32_t			l2_keys		:1;
	ca_uint32_t			l3_keys		:1;
	ca_uint32_t			l4_keys		:1;
	ca_flow_key_l2_mask_t		l2_mask;
	ca_flow_key_l3_mask_t		l3_mask;
	ca_flow_key_l4_mask_t		l4_mask;
} ca_flow_key_mask_t;

typedef struct {
	ca_uint16_t			sw_id[4];
	ca_uint16_t			flow_id;
	ca_flow_key_l2_t		l2_key;
	ca_flow_key_l3_t		l3_key;
	ca_flow_key_l4_t		l4_key;
} ca_flow_key_t;

typedef struct {
	ca_flow_key_type_t		key_type;
	ca_uint8_t			prio;		/* resolution prio in case of multiple hits */
	ca_flow_key_mask_t		key_mask;
} ca_flow_key_type_config_t;

/***************************************************************************
 * Flow Action
 ***************************************************************************/

typedef enum {
	CA_FLOW_VLAN_ACTION_PUSH,	/* push a VLAN tag */
	CA_FLOW_VLAN_ACTION_POP,	/* pop the VLAN tag */
	CA_FLOW_VLAN_ACTION_SWAP,	/* swap the VLAN ID with given */
	CA_FLOW_VLAN_ACTION_SET,	/* set the VLAN ID with given */
} ca_flow_vlan_action_t;

typedef struct {
	ca_uint32_t			flow_id			:1;
	ca_uint32_t			cos			:1;
	ca_uint32_t			dscp			:1;
	ca_uint32_t			decrement_ttl		:1;
	ca_uint32_t			inner_vlan_act		:1;
	ca_uint32_t			inner_dot1p		:1;
	ca_uint32_t			inner_tpid		:1;
	ca_uint32_t			inner_dei		:1;
	ca_uint32_t			outer_vlan_act		:1;
	ca_uint32_t			outer_dot1p		:1;
	ca_uint32_t			outer_tpid		:1;
	ca_uint32_t			outer_dei		:1;
	ca_uint32_t			mac_da			:1;
	ca_uint32_t			mac_sa			:1;
	ca_uint32_t			egress_pppoe_action	:1;
	ca_uint32_t			ip_da			:1;
	ca_uint32_t			ip_sa			:1;
	ca_uint32_t			l4_src_port		:1;
	ca_uint32_t			l4_dest_port		:1;
	ca_uint32_t			sw_id			:1;
} ca_flow_action_option_mask_t;

typedef struct {
	ca_flow_action_option_mask_t	masks;			/* see definition above */
	ca_uint16_t			flow_id;		/* flow ID for following flow based control */
	ca_uint8_t			cos;			/* internal priority */
	ca_uint8_t			dscp;			/* new dscp value for remark */
	ca_flow_vlan_action_t		inner_vlan_act;		/* inner VLAN action */
	ca_uint16_t			inner_dot1p;		/* inner 802.1P for remark or new VLAN tag */
	ca_uint16_t			inner_tpid;		/* inner TPID for remark or new VLAN tag */
	ca_uint16_t			inner_dei;		/* inner DEI for Push/Swap VLAN */
	ca_uint16_t			inner_vid;		/* inner VLAN ID for Push/Swap VLAN */
	ca_flow_vlan_action_t		outer_vlan_act;		/* outer VLAN action */
	ca_uint16_t			outer_dot1p;		/* outer 802.1P for remark or new VLAN tag */
	ca_uint16_t			outer_tpid;		/* outer TPID for remark or new VLAN tag */
	ca_uint16_t			outer_dei;		/* outer DEI for Push/Swap VLAN */
	ca_uint16_t			outer_vid;		/* outer VLAN ID for Push/Swap VLAN */
	ca_mac_addr_t			mac_da;			/* destination MAC */
	ca_mac_addr_t			mac_sa;			/* source MAC */
	ca_uint16_t			pppoe_session_id;	/* valid if pppoe_action is set and if 0, pop the pppoe */
	ca_ip_address_t			ip_da;			/* ip_sa source IP address */
	ca_ip_address_t			ip_sa;			/* ip_da destination IP address */
	ca_uint16_t			l4_src_port;		/* l4 source port */
	ca_uint16_t			l4_dest_port;		/* l4 dest port */
	ca_uint16_t			sw_id[4];		/* store marker value for SW usage */
} ca_flow_action_option_t;

typedef struct {
	ca_classifier_forward_flag_t	forward;
	ca_classifier_action_dest_t	dest;
	ca_flow_action_option_t		options;
} ca_flow_action_t;

/***************************************************************************
 * Flow Definition
 ***************************************************************************/

typedef struct ca_flow {
	ca_uint32_t			idx;		/* unique flow idx returned from driver */
	ca_flow_key_type_t		key_type;	/* flow key type */
	ca_flow_key_t			key;		/* flow key content*/
	ca_flow_action_t		actions;
	ca_uint32_t			aging_time;	/* When ca_flow_add, 0 = static
							 * When ca_flow_get, 0 = static or aging timeout
							 * So caller must be aware that the flow entry is
							 * static or aging entry.
							 * Global NAT entry aging timeout is set via
							 * ca_nat_config_set */
} ca_flow_t;

/***************************************************************************
 * Cortina API
 ***************************************************************************/

ca_status_t ca_flow_key_type_add (
	CA_IN	ca_device_id_t 			device_id,
	CA_IN	ca_flow_key_type_config_t	*p_key_type_config);

ca_status_t ca_flow_key_type_get (
	CA_IN	ca_device_id_t 			device_id,
	CA_OUT	ca_flow_key_type_config_t	*p_key_type_config);

ca_status_t ca_flow_key_type_delete (
	CA_IN	ca_device_id_t			device_id,
	CA_IN	ca_flow_key_type_t		key_type);

ca_status_t ca_flow_aging_time_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		index,
	CA_IN	ca_uint32_t		aging_time);

ca_status_t ca_flow_aging_time_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		index,
	CA_OUT	ca_uint32_t		*p_aging_time);

ca_status_t ca_flow_add (
	CA_IN	ca_device_id_t		device_id,
	CA_OUT	ca_flow_t		*p_flow_config);

ca_status_t ca_flow_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		index,
	CA_OUT	ca_flow_t		*p_flow_config);

ca_status_t ca_flow_iterate (
	CA_IN	ca_device_id_t		device_id,
	CA_OUT	ca_iterator_t		*p_return_entry);

ca_status_t ca_flow_delete (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_uint32_t		index);

ca_status_t ca_flow_delete_all (
	CA_IN	ca_device_id_t		device_id);

#endif /* __FLOW_H__ */
