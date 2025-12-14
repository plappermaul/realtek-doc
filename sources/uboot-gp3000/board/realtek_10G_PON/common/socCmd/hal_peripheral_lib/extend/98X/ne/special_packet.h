#ifndef __SPECIAL_PACKET_H__
#define __SPECIAL_PACKET_H__

#include "ca_types.h"
#include "rx.h"

ca_status_t ca_special_packet_set (
    CA_IN ca_device_id_t      device_id,
    CA_IN ca_port_id_t     source_port,
    CA_IN ca_pkt_type_t special_packet,
    CA_IN ca_boolean_t     forward_to_cpu,
    CA_IN ca_port_id_t     cpu_port,
    CA_IN ca_uint8_t       priority,
    CA_IN ca_uint16_t      flow_id
);

ca_status_t ca_special_packet_get (
    CA_IN  ca_device_id_t      device_id,
    CA_IN  ca_port_id_t     source_port,
    CA_IN  ca_pkt_type_t special_packet,
    CA_OUT ca_boolean_t     *forward_to_cpu,
    CA_OUT ca_port_id_t     *cpu_port,
    CA_OUT ca_uint8_t       *priority,
    CA_OUT ca_uint16_t      *flow_id
);



#endif
