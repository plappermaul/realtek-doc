#ifndef __NAT_H__
#define __NAT_H__

#include "ca_types.h"
#include "api_common.h"

#define CA_NAT_TRANSLATE_SRC_IP		(1 << 0)	/* Enable change SIP     */
#define CA_NAT_TRANSLATE_DST_IP		(1 << 1)	/* Enable change DIP     */
#define CA_NAT_TRANSLATE_L4_PORT	(1 << 2)	/* Enable change L4 port */
#define CA_NAT_TRANSLATE_STATIC		(1 << 3)	/* static flag           */

#define CA_L3_NAT_ID_OVERFLOW_FLAG	0x10000		/* indicate the entry is in overflow table */
#define CA_L3_NAT_HASH_IDX_MASK		0xFFFF		/* mask for hash index for hash table and overflow table */

typedef struct {
	ca_uint32_t	miss_to_cpu;		/* NAT table lookup missing action: 1 - to CPU, 0 - drop                         */
	ca_boolean_t	tcp_ctrl_to_cpu;	/* 1 - Capture FIN/SYN/RST/SYN+ACK pacekts to CPU, 0 - forward as normal traffic */
	ca_uint32_t	aging_time;		/* Unit in seconds                                                               */
} ca_nat_config_t;

typedef struct {
	ca_uint8_t	ip_proto;		/* IP protocol, e.g. TCP or UDP                   */
	ca_ip_address_t	src_ip_addr;		/* Source IP address                              */
	ca_ip_address_t	dst_ip_addr;		/* Destination IP address                         */
	ca_uint16_t	l4_src_port;		/* Source layer 4 port                            */
	ca_uint16_t	l4_dst_port;		/* Destination Layer 4 port                       */
	ca_uint32_t	xlate_flags;		/* Refer to CA_NAT_TRANSLATE_XXX flag definitions */
	ca_ip_address_t	new_src_ip_addr;	/* New source IP address for translation          */
	ca_ip_address_t	new_dst_ip_addr;	/* New destination IP address for translation     */
	ca_uint16_t	new_l4_src_port;	/* New layer 4 source port for translation        */
	ca_uint16_t	new_l4_dst_port;	/* New layer 4 destination port for translation   */
	ca_uint32_t	aging_timer;
	/* For entry get only:
	This field indicates the current hardware aging timer value of the entry. (unit in seconds)
	Global NAT entry aging timeout is set via ca_nat_config_set
	*/
} ca_nat_entry_t;

ca_status_t ca_nat_config_set(
	CA_IN	ca_device_id_t	device_id,
	CA_IN	ca_nat_config_t	*config
);

ca_status_t ca_nat_config_get(
	CA_IN	ca_device_id_t	device_id,
	CA_OUT	ca_nat_config_t	*config
);

ca_status_t ca_nat_entry_add(
	CA_IN	ca_device_id_t	device_id,
	CA_IN	ca_nat_entry_t	*nat_entry
);

ca_status_t ca_nat_entry_get(
	CA_IN	ca_device_id_t	device_id,
	CA_OUT	ca_nat_entry_t	*nat_entry
);

ca_status_t ca_nat_entry_iterate (
	CA_IN	ca_device_id_t	device_id,
	CA_OUT	ca_iterator_t	*p_return_entry
);

ca_status_t ca_nat_entry_delete(
	CA_IN	ca_device_id_t	device_id,
	CA_IN	ca_nat_entry_t	*nat_entry
);

ca_status_t ca_nat_entry_delete_all(
	CA_IN	ca_device_id_t	device_id
);

#endif /* __NAT_H__ */
