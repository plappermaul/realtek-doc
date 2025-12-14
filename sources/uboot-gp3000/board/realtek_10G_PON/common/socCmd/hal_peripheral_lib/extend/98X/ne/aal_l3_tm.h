#ifndef __L3_TM_H__
#define __L3_TM_H__

#include "ca_types.h"
//#include "aal_l3_tm.h"


#define CA_AAL_L3_MAX_PORT_ID      0x7    /* 8 ports */
#if defined(CONFIG_ARCH_CORTINA_G3LITE) || defined(CONFIG_ARCH_CORTINA_SATURN_SFU)
#define CA_AAL_L3_MAX_FLOW_ID      0x7f  /* 128 flows */
#else
#define CA_AAL_L3_MAX_FLOW_ID      0x1ff  /* 512 flows */
#endif
#define CA_AAL_L3_MAX_AGR_FLOW_ID  0x7    /* up to 8 L3 agr flow, each group supports up to 32 flows */
#define CA_AAL_L3_QM_MAX_PORT_ID   0x2f   /* up to 48 logical ports, CPUEPP(0-17), NI(8-15), CPUEPP256(16-47) */


ca_status_t aal_l3_tm_es_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l3_tm_es_ena_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_OUT ca_boolean_t *ena
);

ca_status_t aal_l3_tm_es_cpu_port_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l3_tm_es_cpu_port_ena_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_boolean_t *ena
);

ca_status_t aal_l3_tm_es_ni_port_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l3_tm_es_ni_port_ena_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_boolean_t *ena
);

ca_status_t aal_l3_tm_es_voq_ena_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  voq_id,
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l3_tm_es_voq_ena_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  voq_id,
    CA_OUT ca_boolean_t *ena
);

ca_status_t aal_l3_tm_es_voq_flush_set
(
    CA_IN ca_device_id_t  device_id, 
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  voq_id
);

ca_status_t aal_l3_tm_es_voq_flush_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *voq_id
);

ca_status_t aal_l3_tm_es_voq_status_get
(
    CA_IN  ca_device_id_t  device_id, 
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  voq_id,
    CA_OUT ca_boolean_t *empty
);

ca_status_t aal_l3_tm_voq_drop_ena_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  voq_id,
    CA_IN ca_boolean_t ena
);

ca_status_t aal_l3_tm_voq_drop_ena_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  voq_id,
    CA_OUT ca_boolean_t *ena
);


ca_status_t aal_l3_tm_es_weight_ratio_set
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint8_t   cos,
    CA_IN ca_uint32_t  weight
);

ca_status_t aal_l3_tm_es_weight_ratio_get
(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint8_t   cos,
    CA_OUT ca_uint32_t  *weight
);

ca_status_t aal_l3_tm_init
(   /* not support auto-CLI */
    CA_IN ca_device_id_t device_id
);

#endif 


