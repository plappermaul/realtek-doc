#ifndef __L2_H__
#define __L2_H__

#include "ca_types.h"


#define CA_L2_AGING_TIME_MAX           (0xffe)       /* In seconds */
#define CA_L2_AGING_TIME_NO_AGING      (0xfff)       /* In seconds */
#define CA_L2_ADDR_MAX_NUMS            (0xffff)
#define CA_L2_MAC_FILTER_MASK_MAC_ONLY (0x1)
#define CA_L2_MAC_FILTER_MASK_MAC_VLAN (0x2)

typedef struct{
    ca_mac_addr_t   mac_addr;
    ca_uint32_t     fwd_vid;      /* Indicates egress VID, used for 1:N VLAN support                */
    ca_uint32_t     vid;          /* Indicates the VLAN ID which will take part in look up and
                                     learning logic in the case where the system works in IVL
                                     mode. If system works in SVL mode, this field should be set 
                                     to 0. Valid value range {0x000 ~ 0xfff}                        */
    ca_uint32_t     dot1p;        /* 802.1p priority, {0x0 ~ 0x7}                                   */
    ca_boolean_t    static_flag;  /* Indicates this is a static or dynamic address.                 */
    ca_boolean_t    sa_permit;    /* forward or drop the packet of which SA hits the entry          */
    ca_boolean_t    da_permit;    /* forward or drop the packet of which DA hits the entry          */
    ca_port_id_t    port_id;      /* Indicates the source port id. Set to CA_UINT32_INVALID 
    								 if mc_group_id is valid                                        */
    ca_uint32_t     mc_group_id;  /* Indicates MC group ID. Set to CA_UINT32_INVALID 
                                     if port_id is valid                                            */
    ca_uint32_t     aging_timer;  /* This field is used in getting dynamic address entry.
                                     For entry get, this field indicates the current hardware aging
                                     timer value of the entry. 
                                     Global mac entry aging timeout is set via ca_l2_aging_time_set */
} ca_l2_addr_entry_t;
typedef enum {
    CA_L2_ADDR_OP_STATIC  = 0, /* The deleting operation will touch static entries only.             */
    CA_L2_ADDR_OP_DYNAMIC = 1, /* The deleting operation will touch dynamic entries only.            */
    CA_L2_ADDR_OP_BOTH    = 2, /* The deleting operation will touch both static and dynamic entries. */
    CA_L2_ADDR_OP_OLDEST_DYNAMIC = 3
} ca_l2_addr_op_flags_t;

typedef enum {
    CA_L2_MAC_FULL_FWD_NO_LEARN = 0, /* Forward without learning */
    CA_L2_MAC_FULL_DROP         = 1, /* drop */
    CA_L2_MAC_FULL_FWD_LRU      = 2  /* replace the latest used entry */
} ca_l2_mac_tbl_full_policy_t;

typedef struct{
    ca_boolean_t                lrn_ena;                /* Indicates whether to enable or disable learning function.             */
    ca_boolean_t                drp_pkt_lrn_ena;        /* Indicates whether to learn the packets which are going to be dropped. */
    ca_boolean_t                cpu_special_packet_lrn_ena;     /* Indicates whether to learn MAC SA of to_CPU special packets */
    ca_l2_mac_tbl_full_policy_t sa_mac_tbl_full_polcy;  /* Indicates learning and forwarding behavior when MAC address number reaches
                                                           the limit threshold or MAC table is full.                             */
} ca_l2_learning_control_t;

typedef enum {
    CA_L2_LEARNING_DISABLE  = 0, /* Learning function is diabled. All L2 address shall not be learned in system level. */
    CA_L2_LEARNING_SOFTWARE = 1, /* System will generate event and notify CPU of the new L2 address, software needs to
                                    manually maintain L2 FDB table.                                                    */
    CA_L2_LEARNING_HARDWARE = 2, /* Hardware will automatically learn L2 address.                                      */
    CA_L2_LEARNING_MAX
} ca_l2_learning_mode_t;

typedef enum {
    CA_L2_AGING_HARDWARE	= 0,
    CA_L2_AGING_SOFTWARE	= 1
} ca_l2_aging_mode_t;

typedef enum {
    CA_L2_MAC_LIMIT_PORT   = 0, /* MAC limit function is based on port.   */
    CA_L2_MAC_LIMIT_VLAN   = 1, /* MAC limit function is based on vlan.   */
    CA_L2_MAC_LIMIT_SYSTEM = 2, /* MAC limit function is based on system. */
    CA_L2_MAC_LIMIT_DISABLE	= 3
} ca_l2_mac_limit_type_t;

typedef enum {
    CA_L2_FLOODING_TYPE_MC  = 0, /* multicast       */
    CA_L2_FLOODING_TYPE_BC  = 1, /* broadcast       */
    CA_L2_FLOODING_TYPE_UUC = 2, /* unknown unicast */
} ca_l2_flooding_type_t;

typedef struct {
    ca_uint32_t   mask;      /* Refer to CA_L2_MAC_FILTER_MASK_xxx */
    ca_mac_addr_t mac;       /* MAC address for filter function */
    ca_uint32_t   mac_flag;  /* Indicates the filter is based on source or destination*/
                             /* {0(SA), 1(DA)}*/
    ca_uint32_t   vid;       /* Top VLAN tag, only used when filter mode is MAC+VLAN                */
    ca_boolean_t  drop_flag; /* Indciates whether to drop the packet. {FALSE(fordward), TRUE(drop)} */
} ca_l2_mac_filter_entry_t;

typedef struct {
    ca_port_id_t	     	    port_id;	/* port id */
    ca_l2_mac_filter_entry_t	mac_filter;	/* mac filter */
} ca_l2_mac_filter_entry_iterator_t;


typedef int (/*CODEGEN_IGNORE_TAG*/*ca_l2_addr_filter_traverse_cb)(ca_device_id_t dev, ca_l2_mac_filter_entry_t *filter, ca_port_id_t *port_id, void *user_data);

typedef int (/*CODEGEN_IGNORE_TAG*/*ca_l2_addr_traverse_cb)(ca_device_id_t dev, ca_l2_addr_entry_t *addr, void *user_data);

ca_status_t ca_l2_learning_control_set (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_port_id_t             port_id,
    CA_IN ca_l2_learning_control_t *control
);

ca_status_t ca_l2_learning_control_get (
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_port_id_t             port_id,
    CA_OUT ca_l2_learning_control_t *control
);

ca_status_t ca_l2_learning_mode_set (
    CA_IN  ca_device_id_t           device_id,
    CA_IN  ca_l2_learning_mode_t mode
);

ca_status_t ca_l2_learning_mode_get (
    CA_IN  ca_device_id_t             device_id,
    CA_OUT ca_l2_learning_mode_t   *mode);

ca_status_t ca_l2_aging_mode_set (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_l2_aging_mode_t        mode);
ca_status_t ca_l2_aging_mode_get (
    CA_IN ca_device_id_t            device_id,
    CA_OUT ca_l2_aging_mode_t       *mode);

ca_status_t ca_l2_mac_limit_set (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_l2_mac_limit_type_t type,
    CA_IN ca_uint32_t            data,
    CA_IN ca_uint32_t            number
);

ca_status_t ca_l2_mac_limit_get (
    CA_IN  ca_device_id_t            device_id,
    CA_IN  ca_l2_mac_limit_type_t type,
    CA_IN  ca_uint32_t            data,
    CA_OUT ca_uint32_t           *number
);

ca_status_t ca_l2_aging_time_set (
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t time
);

ca_status_t ca_l2_aging_time_get (
    CA_IN  ca_device_id_t device_id,
    CA_OUT ca_uint32_t *time
);

ca_status_t ca_l2_addr_add (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_l2_addr_entry_t *addr
);

ca_status_t ca_l2_addr_delete (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_l2_addr_entry_t *addr
);

ca_status_t ca_l2_addr_delete_by_port (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_port_id_t          port_id,
    CA_IN ca_l2_addr_op_flags_t flag
);

ca_status_t ca_l2_addr_delete_by_mac (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_mac_addr_t         mac,
    CA_IN ca_l2_addr_op_flags_t flag
);

ca_status_t ca_l2_addr_delete_by_vlan (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_vlan_id_t          vid,
    CA_IN ca_l2_addr_op_flags_t flag
);

ca_status_t ca_l2_addr_get (
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_mac_addr_t      mac,
    CA_IN  ca_vlan_id_t       vid,
    CA_OUT ca_l2_addr_entry_t *addr
);

ca_status_t ca_l2_addr_iterate (
    CA_IN  ca_device_id_t    device_id,
    CA_OUT ca_iterator_t *p_return_entry
);

ca_status_t ca_l2_addr_delete_all (
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_l2_addr_op_flags_t flag
);

ca_status_t ca_l2_addr_aged_event_get (
    CA_IN ca_device_id_t         device_id,
    CA_OUT ca_l2_addr_entry_t    *addr);

ca_status_t ca_l2_mac_filter_default_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_boolean_t drop_flag
);

ca_status_t ca_l2_mac_filter_default_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_boolean_t *drop_flag
);

ca_status_t ca_l2_mac_filter_add (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_port_id_t             port_id,
    CA_IN ca_l2_mac_filter_entry_t *entry
);

ca_status_t ca_l2_mac_filter_delete (
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_port_id_t             port_id,
    CA_IN ca_l2_mac_filter_entry_t *entry
);

ca_status_t ca_l2_mac_filter_delete_all (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id
);

ca_status_t ca_l2_mac_filter_iterate (
    CA_IN  ca_device_id_t                  device_id,
    CA_OUT ca_iterator_t                *p_return_entry
);

ca_status_t ca_l2_flooding_rate_set(
    CA_IN ca_device_id_t           device_id,
    CA_IN ca_port_id_t          port_id,
    CA_IN ca_l2_flooding_type_t ptype,
    CA_IN ca_uint32_t           *rate
);

ca_status_t ca_l2_flooding_rate_get(
    CA_IN  ca_device_id_t           device_id,
    CA_IN  ca_port_id_t          port_id,
    CA_IN  ca_l2_flooding_type_t ptype,
    CA_OUT ca_uint32_t           *rate
);


#endif
