#ifndef __CA_TPUT_H__
#define __CA_TPUT_H__

#include "ca_types.h"

typedef struct ca_tput_upstream_flow_spec_s {
	ca_ip_address_t	src_ip_addr;	/* Source IP address */
	ca_uint16_t		l4_src_port;	/* Source layer 4 port */
	ca_ip_address_t	dst_ip_addr;	/* Destination IP address */
	ca_uint16_t		l4_dst_port;	/* Destination Layer 4 port */
	ca_uint8_t 		ip_proto;	/* IP protocol, e.g. TCP or UDP */

	/*the information of ip hdr*/
	ca_uint8_t    		tos;
	ca_uint16_t   		tot_len;
	ca_uint16_t   		id;
	ca_uint16_t   		frag_off;
	ca_uint16_t   		ttl;

	ca_uint32_t 		tx_rate; 	/*L3 throughput rate (uint is Mbps)*/
	ca_uint32_t 		tx_count;	/*tx packet count*/
} ca_tput_upstream_flow_spec_t;

typedef struct ca_tput_downstream_flow_spec_s {
	ca_ip_address_t	src_ip_addr;	/* Source IP address */
	ca_uint16_t		l4_src_port;	/* Source layer 4 port */
	ca_ip_address_t	dst_ip_addr;	/* Destination IP address */
	ca_uint16_t		l4_dst_port;	/* Destination Layer 4 port */
	ca_uint8_t 		ip_proto;	/* IP protocol, e.g. TCP or UDP */

	ca_uint32_t 		rx_count; 	/* the expected rx packet count*/
} ca_tput_downstream_flow_spec_t;


/***************************************************************************
 * TPUT functions - Generic
 ***************************************************************************/
ca_status_t ca_tput_upstream_test_start (
	CA_IN		ca_device_id_t 					device_id,
	CA_IN		ca_uint8_t						flow_count,
	CA_IN		ca_tput_upstream_flow_spec_t*	flows);

ca_status_t ca_tput_upstream_test_stats_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN_OUT	ca_uint32_t		*p_packet_sent);

ca_status_t ca_tput_downstream_test_start (
	CA_IN		ca_device_id_t 					device_id,
	CA_IN		ca_uint8_t						flow_count,
	CA_IN_OUT	ca_tput_downstream_flow_spec_t*	flows);

ca_status_t ca_tput_downstream_test_stats_get (
	CA_IN		ca_device_id_t		device_id,
	CA_IN_OUT	ca_uint32_t		*p_packet_received);

ca_status_t ca_tput_downstream_test_stop (
	CA_IN		ca_device_id_t		device_id) ;

/***************************************************************************
 * Initialization
 ***************************************************************************/
ca_status_t ca_tput_init(CA_IN ca_device_id_t  device_id);


#endif /* __CA_TPUT_H__ */

