#ifndef __VLAN_H__
#define __VLAN_H__

#include "ca_types.h"
#include "sys.h"

typedef enum {
    CA_L2_VLAN_TAG_TYPE_CVLAN = 0,
    CA_L2_VLAN_TAG_TYPE_SVLAN = 1,
} ca_vlan_tag_type_t;

typedef struct{
    ca_boolean_t drop_unknown_vlan;              /* drop or permit the unknown VLAN packets.      */
    ca_boolean_t ingr_membership_check_enable;   /* check or not to check ingress VLAN membership {FALSE, TRUE}  */
    ca_boolean_t egr_membership_check_enable;    /* check or not to check egress VLAN membership {FALSE, TRUE}  */
    ca_boolean_t ingress_vlan_action_enable;     /* ingress vlan action control                   */
    ca_boolean_t egress_vlan_action_enable;      /* egress vlan action control                    */
    ca_boolean_t default_tag_add;                /* add default VLAN for all incoming packets (untagged and tagged). */
    ca_vlan_id_t default_tag;				    /* valid only if default_tag_add = TRUE*/
    ca_vlan_tag_type_t outer_tpid;
    ca_vlan_tag_type_t inner_tpid;
} ca_vlan_port_control_t;

typedef struct {
    ca_uint32_t          outer_pbits    :1;  
    ca_uint32_t          inner_pbits    :1;  
    ca_uint32_t          outer_tpid     :1;  
    ca_uint32_t          inner_tpid     :1;  
    ca_uint32_t          outer_dei      :1;  
    ca_uint32_t          inner_dei      :1;  
    ca_uint32_t          outer_vid      :1;  
    ca_uint32_t          inner_vid      :1;  
} ca_vlan_key_type_t;


typedef enum {
    CA_L2_VLAN_TAG_ACTION_NOP  = 0,
    CA_L2_VLAN_TAG_ACTION_PUSH = 1,
    CA_L2_VLAN_TAG_ACTION_POP  = 2,
    CA_L2_VLAN_TAG_ACTION_SWAP = 3
} ca_vlan_tag_action_t;

typedef enum {
    CA_L2_VLAN_TPID_LOCAL      = 0,
    CA_L2_VLAN_TPID_INNER_TAG  = 1,
    CA_L2_VLAN_TPID_OUTER_TAG  = 2
} ca_vlan_tpid_source_t;

typedef enum {   
    CA_L2_VLAN_NEW_VLAN_SRC_LOCAL = 0 , /* VLAN from field new_outer_vlan */
    CA_L2_VLAN_NEW_VLAN_SRC_FDB   = 1 /* VLAN from FDB                  */
} ca_vlan_new_vlan_source_t;

typedef enum {
    CA_L2_VLAN_TAG_PRI_SRC_NEW_PRI              = 0,
    CA_L2_VLAN_TAG_PRI_SRC_INNER_PRI            = 1,
    CA_L2_VLAN_TAG_PRI_SRC_OUTER_PRI            = 2,
    CA_L2_VLAN_TAG_PRI_SRC_DSCP_TO_8021P_TABLE  = 3,
    CA_L2_VLAN_TAG_PRI_SRC_8021P_REMARK_TABLE   = 4,
    CA_L2_VLAN_TAG_PRI_SRC_8021P_TO_DSCP_TABLE  = 5,
} ca_vlan_tag_priority_source_t;

typedef enum {
    CA_L2_VLAN_LEARNING_IVL 	= 0,	/* Independent VLAN learning		*/
    CA_L2_VLAN_LEARNING_SVL 	= 1,	/* Shared VLAN learning 		*/
    CA_L2_VLAN_LEARNING_DIS 	= 2,	/* disable VLAN learning 		*/
} ca_vlan_learning_mode_t;

typedef enum {
    CA_L2_ING_PORT_TYPE_WAN = 0,
    CA_L2_ING_PORT_TYPE_LAN = 1,
} ca_vlan_ing_port_type_t;

typedef enum {
    CA_L2_VLAN_ACTION_DIR_LAN_TO_WAN = 0,
    CA_L2_VLAN_ACTION_DIR_WAN_TO_LAN = 1,
} ca_vlan_action_direction_t;


typedef struct {
    ca_vlan_id_t                  new_inner_vlan;       /* new VLAN ID for push or swap operation                      */
    ca_vlan_new_vlan_source_t     new_inner_vlan_src;   /* used to select new inner VLAN, refer to
                                                           enum ca_vlan_new_vlan_source_t                              */
    ca_vlan_tag_priority_source_t new_inner_pri_src;    /* Indicates the source for inner VLAN tag                     */
    ca_uint32_t                   new_inner_pri;        /* new inner 802.1p priority                                   */
    ca_uint32_t                   new_inner_tpid_index; /* system supports up to 4 TPID(s), one of them
                                                           can be selected to be the TPID for VLAN action -
                                                           PUSH and SWAP.( new_inner_tpid_src== CA_L2_VLAN_TPID_LOCAL) */
    ca_vlan_tpid_source_t         new_inner_tpid_src;   /* Indicates the source for tpid of inner tag                  */
    ca_vlan_tag_action_t          inner_vlan_cmd;       /* refer to enum ca_vlan_tag_action_t                          */
    ca_vlan_id_t                  new_outer_vlan;       /* new VLAN ID for push or swap operation                      */
    ca_vlan_new_vlan_source_t     new_outer_vlan_src;   /* used to select new outer VLAN, refer to
                                                           enum ca_vlan_new_vlan_source_t                              */
    ca_uint32_t                   new_outer_pri;        /* new outer 802.1p priority                                   */
    ca_vlan_tag_priority_source_t new_outer_pri_src;    /* Indicates the source for outer VLAN tag                     */
    ca_uint32_t                   new_outer_tpid_index; /* system supports up to 4 TPID(s), one of them  can be selected
                                                           to be the TPID for outer VLAN action - PUSH and SWAP.
                                                           ( new_inner_tpid_src== CA_L2_VLAN_TPID_LOCAL)               */
    ca_vlan_tpid_source_t         new_outer_tpid_src;   /* Indicates the source for tpid of outer tag                  */
    ca_vlan_tag_action_t          outer_vlan_cmd;       /* refer to enum ca_vlan_tag_action_t                          */

    ca_uint16_t				      flow_id;	
} ca_vlan_action_t;

typedef struct{
     ca_uint32_t  vid;
     ca_uint8_t   member_count;
     ca_port_id_t member_ports;
     ca_uint8_t   untagged_count;
     ca_port_id_t  untagged_ports;

}ca_vlan_entry_t;

typedef struct {
    ca_uint16_t			vid;		/* 12 bit VLAN ID */
    ca_uint8_t			member_count;
    ca_port_id_t		member_ports[CA_MAX_PORT_NUM];
    ca_uint8_t			untagged_count;
    ca_port_id_t		untagged_ports[CA_MAX_PORT_NUM];
} ca_vlan_iterate_entry_t;

typedef struct {
    ca_vlan_id_t		egress_vid;	/* specify CA_UINT16_INVALID for untagged */
    ca_port_id_t		egress_port;	/* egress port */
} ca_flooding_port_t;


typedef struct {
    ca_vlan_key_type_t      key_type;   /*vlan key type*/
    ca_uint8_t              outer_pbits;    /*outer pbits for key matching*/
    ca_uint8_t              inner_pbits;    /*innner pbits for key matching*/
    ca_uint16_t             outer_tpid; /*outer tpid for key matching*/
    ca_uint16_t             inner_tpid;  /*inner tpid for key matching*/
    ca_uint16_t             outer_dei;  /*outer dei for key matching*/
    ca_uint16_t             inner_dei;    /*inner dei for key matching*/
    ca_uint16_t             outer_vid;  /*outer vid for key matching*/
    ca_uint16_t             inner_vid;    /*inner vid for key matching*/
} ca_vlan_key_entry_t;

typedef struct {
    ca_vlan_key_entry_t		key;/*vlan action key*/
    ca_vlan_action_t		action;
} ca_vlan_action_iterate_entry_t;


typedef ca_int32 (/*CODEGEN_IGNORE_TAG*/*ca_l2_vlan_traverse_cb) (ca_device_id_t dev, ca_uint32_t vid, void *user_data);

ca_status_t ca_l2_vlan_learning_shared_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_vlan_learning_mode_t mode
);

ca_status_t ca_l2_vlan_learning_shared_get (
    CA_IN  ca_device_id_t  device_id,
    CA_OUT ca_vlan_learning_mode_t *mode
);

ca_status_t ca_l2_vlan_port_control_set (
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_port_id_t           port_id,
    CA_IN ca_vlan_port_control_t *config
);

ca_status_t ca_l2_vlan_port_control_get (
    CA_IN  ca_device_id_t            device_id,
    CA_IN  ca_port_id_t           port_id,
    CA_OUT ca_vlan_port_control_t *config
);

ca_status_t ca_l2_vlan_tpid_set (
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t *inner_tpid, /*[5:5]*/
    CA_IN ca_uint32_t inner_nums,
    CA_IN ca_uint32_t *outer_tpid, /*[5:5]*/
    CA_IN ca_uint32_t outer_nums
);

ca_status_t ca_l2_vlan_tpid_get (
    CA_IN  ca_device_id_t device_id,
    CA_OUT ca_uint32_t *inner_tpid,/*[5:5]*/
    CA_OUT ca_uint32_t *inner_nums,
    CA_OUT ca_uint32_t *outer_tpid,/*[5:5]*/
    CA_OUT ca_uint32_t *outer_nums
);

ca_status_t ca_l2_vlan_outer_tpid_add (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
);

ca_status_t ca_l2_vlan_outer_tpid_delete (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
);

ca_status_t ca_l2_vlan_inner_tpid_add (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
);

ca_status_t ca_l2_vlan_inner_tpid_delete (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  tpid_sel
);

ca_status_t ca_l2_vlan_outer_tpid_list (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t  *tpid,/*[4:4]*/
    CA_OUT ca_uint32_t  *tpid_nums
);

ca_status_t ca_l2_vlan_inner_tpid_list (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_uint32_t *tpid,/*[4:4]*/
    CA_OUT ca_uint32_t *tpid_nums
);

ca_status_t ca_l2_vlan_create (
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t vid
);

ca_status_t ca_l2_vlan_delete (
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t vid
);

ca_status_t ca_l2_vlan_delete_all (
    CA_IN ca_device_id_t device_id
);

ca_status_t ca_l2_vlan_port_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_uint32_t  vid,
    CA_IN ca_uint8_t   member_count,
    CA_IN ca_port_id_t *member_ports, /*[8:8]*/
    CA_IN ca_uint8_t   untagged_count,
    CA_IN ca_port_id_t *untagged_ports  /*[3:3]*/
);

ca_status_t ca_l2_vlan_port_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint32_t  vid,
    CA_OUT ca_uint8_t   *member_count,
    CA_OUT ca_port_id_t *member_ports, /*[8:8]*/
    CA_OUT ca_uint8_t   *untagged_count,
    CA_OUT ca_port_id_t * untagged_ports /*[8:8]*/
);

ca_status_t ca_l2_vlan_iterate (
    CA_IN 		ca_device_id_t		device_id,
    CA_IN_OUT 	ca_iterator_t	*p_return_entry
);


ca_status_t ca_l2_vlan_ingress_default_action_set (
    CA_IN ca_device_id_t       device_id,
    CA_IN ca_port_id_t      port_id,
    CA_IN ca_vlan_action_t *untag_action,
    CA_IN ca_vlan_action_t *single_tag_action,
    CA_IN ca_vlan_action_t *double_tag_action
);

ca_status_t ca_l2_vlan_ingress_default_action_get (
    CA_IN  ca_device_id_t      device_id,
    CA_IN  ca_port_id_t     port_id,
    CA_OUT ca_vlan_action_t *untag_action,
    CA_OUT ca_vlan_action_t *single_tag_action,
    CA_OUT ca_vlan_action_t *double_tag_action
);

ca_status_t ca_l2_vlan_ingress_action_add (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t    *key,
    CA_IN ca_vlan_action_t   *action
);

ca_status_t ca_l2_vlan_ingress_action_delete (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t    key
);

ca_status_t ca_l2_vlan_ingress_action_delete_all (
	CA_IN		ca_device_id_t				device_id,
	CA_IN		ca_port_id_t			port_id);


ca_status_t ca_l2_vlan_ingress_action_get (
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_port_id_t       port_id,
    CA_IN  ca_vlan_key_entry_t    key,
    CA_OUT ca_vlan_action_t   *action
);

ca_status_t ca_l2_vlan_ingress_action_iterate (
    CA_IN		ca_device_id_t		device_id,
    CA_IN		ca_port_id_t	port_id,
    CA_IN_OUT	ca_iterator_t	*p_return_entry
);

ca_status_t ca_l2_vlan_egress_action_add (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t   *key,
    CA_IN ca_vlan_action_t   *action
);

ca_status_t ca_l2_vlan_egress_action_delete (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_port_id_t       port_id,
    CA_IN ca_vlan_key_entry_t    key
);

ca_status_t ca_l2_vlan_egress_action_delete_all (
	CA_IN		ca_device_id_t				device_id,
	CA_IN		ca_port_id_t			port_id);

ca_status_t ca_l2_vlan_egress_action_get (
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_port_id_t       port_id,
    CA_IN  ca_vlan_key_entry_t   key,
    CA_OUT ca_vlan_action_t   *action
);

ca_status_t ca_l2_vlan_egress_default_action_set(
    CA_IN       ca_device_id_t         device_id,
    CA_IN       ca_port_id_t        port_id,
    CA_IN       ca_vlan_action_t    *default_action,
    CA_IN       ca_vlan_action_t    *untag_action,
    CA_IN       ca_vlan_action_t    *single_tag_action,
    CA_IN       ca_vlan_action_t    *double_tag_action
);

ca_status_t ca_l2_vlan_egress_default_action_get (
    CA_IN       ca_device_id_t             device_id,
    CA_IN       ca_port_id_t            port_id,
    CA_OUT      ca_vlan_action_t        *default_action,
    CA_OUT      ca_vlan_action_t        *untag_action,
    CA_OUT      ca_vlan_action_t        *single_tag_action,
    CA_OUT      ca_vlan_action_t        *double_tag_action
);
ca_status_t ca_l2_vlan_traverse (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t            device_id,
    CA_IN ca_l2_vlan_traverse_cb trav_fn,
    CA_IN void                   *user_data
);

ca_status_t ca_l2_vlan_egress_action_iterate (
    CA_IN		ca_device_id_t		device_id,
    CA_IN		ca_port_id_t	port_id,
    CA_IN_OUT	ca_iterator_t	*p_return_entry
);

ca_status_t ca_l2_vlan_flooding_domain_members_add (
    CA_IN		ca_device_id_t			device_id,
    CA_IN		ca_vlan_id_t		ingress_vid,
    CA_IN		ca_uint8_t			ingress_port_type,
    CA_IN		ca_uint8_t			member_count,
    CA_IN		ca_flooding_port_t	*members /*[8:8]*/
);

ca_status_t ca_l2_vlan_flooding_domain_members_update (
    CA_IN		ca_device_id_t			device_id,
    CA_IN		ca_vlan_id_t		ingress_vid,
    CA_IN		ca_uint8_t			ingress_port_type,
    CA_IN		ca_uint8_t			member_count,
    CA_IN		ca_flooding_port_t	*members /*[8:8]*/
);

ca_status_t ca_l2_vlan_flooding_domain_members_delete (
    CA_IN		ca_device_id_t			device_id,
    CA_IN		ca_vlan_id_t		ingress_vid,
    CA_IN		ca_uint8_t			ingress_port_type);

ca_status_t ca_l2_vlan_flooding_domain_members_get (
    CA_IN		ca_device_id_t			device_id,
    CA_IN		ca_vlan_id_t		ingress_vid,
    CA_IN		ca_uint8_t			ingress_port_type,
    CA_OUT		ca_uint8_t			*p_member_count,
    CA_OUT		ca_flooding_port_t	*members /*[8:8]*/
);

ca_status_t ca_l2_vlan_action_get (
    CA_IN		ca_device_id_t			device_id,
    CA_IN		ca_vlan_id_t		ingress_vid,
    CA_IN		ca_boolean_t		direction,
    CA_OUT		ca_vlan_action_t	*p_vlan_action);

ca_status_t ca_l2_vlan_action_set (
    CA_IN		ca_device_id_t			device_id,
    CA_IN		ca_vlan_id_t		ingress_vid,
    CA_IN		ca_boolean_t		direction,
    CA_IN		ca_vlan_action_t	*p_vlan_action);


#endif
