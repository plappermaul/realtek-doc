
#ifndef __CA_MULTICAST_H__
#define __CA_MULTICAST_H__


#include "ca_types.h"
#include "vlan.h"

#ifdef __cplusplus
extern "C"{
#endif

#define CA_MAX_L2_MC_MEMBER 32
#define CA_MAX_L3_MC_MEMBER 32
#define CA_MC_EGRESS_ACTION_RESERVED 4

typedef enum {
	CA_MC_MODE_L2 = 1,	/* filter, replicate based on MAC DA */
	CA_MC_MODE_L3 = 2	/* filter, replicate based on IP DA */
} ca_mcast_mode_t;

typedef struct  {
	ca_boolean_t					mcast_flooding_enable;
	ca_mcast_mode_t					mode;
} ca_l2_mcast_config_t;

typedef struct {
	ca_uint32_t					mac_da_overwrite	:1;
	ca_uint32_t					vlan_action		:1;
} ca_mcast_egress_action_mask_t;

typedef struct ca_l2_mcast_entry_s {
	ca_uint32_t					mcg_id;
	ca_uint16_t					mcast_vlan;
	ca_ip_address_t					group_ip_addr;
	ca_mac_addr_t					group_mac_addr;
	/* src_ip_address.addr_len = 0 for (*,G)*/
	ca_ip_address_t					src_ip_address;
} ca_l2_mcast_entry_t;

typedef struct ca_l2_mcast_member_s {
	ca_mcast_egress_action_mask_t			action_mask;
	ca_mac_addr_t 					new_mac_da;
	ca_vlan_tag_action_t				vlan_action;
	ca_uint16_t					vid;
	ca_uint32_t 					reserved[CA_MC_EGRESS_ACTION_RESERVED];
	ca_port_id_t					member_port;
} ca_l2_mcast_member_t;

typedef struct ca_l2_mcast_group_members_s {
	ca_uint32_t					mcg_id;
	ca_uint32_t					member_count;
	ca_l2_mcast_member_t				member[CA_MAX_L2_MC_MEMBER];
} ca_l2_mcast_group_members_t;

typedef struct ca_l2_mcast_iterator_s {
	ca_l2_mcast_entry_t 				mc_entry;
	ca_l2_mcast_group_members_t			members;
} ca_l2_mcast_iterator_entry_t;

typedef struct ca_l3_mcast_entry_s {
	/*unique ID for multicast traffic replication returned by driver*/
	ca_uint32_t					mcg_id;
	ca_uint16_t					mcast_vlan;
	ca_ip_address_t					group_ip_addr;
	/* src_ip_address.addr_len = 0 for (*,G)*/
	ca_ip_address_t					src_ip_address;
} ca_l3_mcast_entry_t;

typedef struct ca_l3_mcast_member_s {
	ca_mcast_egress_action_mask_t 			action_mask;
	ca_mac_addr_t 					new_mac_da;
	ca_uint32_t 					reserved[CA_MC_EGRESS_ACTION_RESERVED];
	ca_intf_id_t					member_intf;
} ca_l3_mcast_member_t;

typedef struct ca_l3_mcast_group_members_s {
	ca_uint32_t					mcg_id;
	ca_uint32_t					member_count;
	ca_l3_mcast_member_t				member[CA_MAX_L3_MC_MEMBER];
} ca_l3_mcast_group_members_t;

typedef struct ca_l3_mcast_iterator_s {
	ca_l3_mcast_entry_t 				mc_entry;
	ca_l3_mcast_group_members_t			members;
} ca_l3_mcast_iterator_entry_t;

ca_status_t ca_mcast_config_set (
	CA_IN ca_device_id_t		device_id,
	CA_IN ca_l2_mcast_config_t	*l2_mcast_config
);
ca_status_t ca_mcast_config_get (
	CA_IN  ca_device_id_t		device_id,
	CA_OUT ca_l2_mcast_config_t	*l2_mcast_config
);

ca_status_t ca_l2_mcast_group_add (
	CA_IN ca_device_id_t		device_id,
	CA_OUT ca_l2_mcast_entry_t	*entry
);

ca_status_t ca_l2_mcast_group_delete (
	CA_IN ca_device_id_t		device_id,
	CA_IN ca_uint32_t		mcg_id
);

ca_status_t ca_l2_mcast_group_delete_all (
	CA_IN ca_device_id_t		device_id
);

ca_status_t ca_l2_mcast_group_get (
	CA_IN ca_device_id_t		device_id,
	CA_IN ca_uint32_t		mcg_id,
	CA_OUT ca_l2_mcast_iterator_entry_t	*mcast_group
);

ca_status_t ca_l2_mcast_group_iterate (
	CA_IN ca_device_id_t 		device_id,
	CA_OUT ca_iterator_t		*p_return_entry
);

ca_status_t ca_l2_mcast_member_add (
	CA_IN ca_device_id_t 			device_id,
	CA_IN ca_l2_mcast_group_members_t	*member
);

ca_status_t ca_l2_mcast_member_delete (
	CA_IN ca_device_id_t 			device_id,
	CA_IN ca_l2_mcast_group_members_t	*member
);

ca_status_t ca_l3_mcast_group_add (
	CA_IN ca_device_id_t         device_id,
	CA_OUT ca_l3_mcast_entry_t *entry
);

ca_status_t ca_l3_mcast_group_delete (
	CA_IN ca_device_id_t		device_id,
	CA_IN ca_uint32_t		mcg_id
);

ca_status_t ca_l3_mcast_group_delete_all (
	CA_IN ca_device_id_t		device_id
);

ca_status_t ca_l3_mcast_group_get (
	CA_IN ca_device_id_t		device_id,
	CA_IN ca_uint32_t		mcg_id,
	CA_OUT ca_l3_mcast_iterator_entry_t	*mcast_group
);

ca_status_t ca_l3_mcast_group_iterate (
	CA_IN ca_device_id_t 		device_id,
	CA_OUT ca_iterator_t		*p_return_entry
);

ca_status_t ca_l3_mcast_member_add (
	CA_IN ca_device_id_t 			device_id,
	CA_IN ca_l3_mcast_group_members_t	*member
);

ca_status_t ca_l3_mcast_member_delete (
	CA_IN ca_device_id_t 			device_id,
	CA_IN ca_l3_mcast_group_members_t	*member
);

/* Test functions */
ca_status_t mcast_l2_add_1(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t 			device_id
);
ca_status_t mcast_l2_add_2(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t 			device_id
);
ca_status_t mcast_l2_mb_del_1(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t                    device_id,
	CA_IN ca_uint32_t                    mcg_id
);
ca_status_t mcast_l2_get(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id,
	CA_IN ca_uint32_t mcg_id
);
ca_status_t mcast_l2_itr(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id
);
ca_status_t intf_l3_add_1(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t 			device_id
);
ca_status_t mcast_l3_add_1(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t 			device_id
);
ca_status_t mcast_l3_add_2(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t 			device_id
);
ca_status_t mcast_l3_mb_del_1(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id,
	CA_IN ca_uint32_t mcg_id
);
ca_status_t mcast_l3_get(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id,
	CA_IN ca_uint32_t mcg_id
);
ca_status_t mcast_l3_itr(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id
);
ca_status_t flooding_set(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id,
	CA_IN ca_boolean_t flooding
);
ca_status_t flooding_get(/*CODEGEN_IGNORE_TAG*/
	CA_IN ca_device_id_t device_id
);

#ifdef __cplusplus
}
#endif

#endif /* __CA_MULTICAST_H__ */
