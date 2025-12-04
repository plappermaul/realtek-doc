#ifndef __MIRROR_H__
#define __MIRROR_H__

#include "ca_types.h"


ca_status_t ca_mirror_control_set(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t src_port,
    CA_IN ca_port_id_t dst_port,
    CA_IN ca_boolean_t mir_ena);

#endif
