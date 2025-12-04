#ifndef __CLASSIFIER_H__
#define __CLASSIFIER_H__

#include "ca_types.h"

#define CA_MAX_DATA_EXTRACTION_LEN 8

typedef struct {
    ca_uint32_t min;   /* inclusive small boundary                          */
    ca_uint32_t max;   /* inclusive big boundary, end = start for one value */
} ca_classifier_range_t;

typedef struct {
    ca_uint16_t tpid; /* TPID        */
    ca_uint8_t  pri;  /* 802.1p, PCP */
    ca_uint8_t  dei;  /* CFI or DEI  */
    ca_uint16_t vid;  /* VLAN ID     */
} ca_classifier_vlan_t;

typedef struct{
    ca_uint32_t tpid : 1;
    ca_uint32_t vid  : 1;
    ca_uint32_t dei  : 1;
    ca_uint32_t pri  : 1;
} ca_classifier_vlan_mask_t;

typedef struct {
    ca_uint16_t              ethertype;   		/* ethertype value */
    ca_uint16_t              subtype;   		/* ethertype subtype or opcode value, OAM/MPCP */
    ca_mac_addr_t        src_mac;			/* source MAC address */
    ca_mac_addr_t        dst_mac;			/* destination MAC address */
    ca_classifier_vlan_t vlan_otag;		/* the outer VLAN tag */
    ca_classifier_vlan_t vlan_itag;		/* the inner VLAN tag */
    ca_uint8_t                vlan_count;		/* tag count */
    ca_boolean_t           is_multicast;		/* true - multicast, false - non-multicast */
    ca_boolean_t           is_length;   		/* true - length, false - ethertype */
    ca_uint16_t              cfm_opcode;   	/* CFM IPcode, work with 802.1AG ethertype */
} ca_classifier_l2_t;

typedef struct {
    ca_uint32_t                        ethertype		:1; 
    ca_uint32_t                        subtype			:1; 
    ca_uint32_t                        src_mac			:6;	/*each bit enable one byte */
    ca_uint32_t                        dst_mac			:6;	/*each bit enable one byte */
    ca_uint32_t                        vlan_otag		:1;	/* outer tag */
    ca_uint32_t                        vlan_itag		        :1;	/* inner tag */
    ca_uint32_t                        vlan_count		:1;
    ca_uint32_t                        is_multicast		:1;
    ca_uint32_t                        is_length		        :1; 
    ca_uint32_t                        cfm_opcode		:1; 

    /* when vlan_count = 1, top tag is the otag */
    ca_classifier_vlan_mask_t	vlan_otag_mask; 
    ca_classifier_vlan_mask_t	vlan_itag_mask; 
} ca_classifier_l2_mask_t;

typedef enum {
    CA_IPV6_EXT_HEADER_HOP_BY_HOP,       /* hop-by-hop options header             */
    CA_IPV6_EXT_HEADER_ROUTING,          /* routing header                        */
    CA_IPV6_EXT_HEADER_FRAGMENT,         /* fragment header                       */
    CA_IPV6_EXT_HEADER_DESTINATION,      /* destination options header            */
    CA_IPV6_EXT_HEADER_AUTHENTICATION,   /* authentication header                 */
    CA_IPV6_EXT_HEADER_ESP               /* encapsulating security payload header */
} ca_ipv6_ext_header_t;


typedef struct {
    ca_boolean_t          l4_valid;      /* layer 4 or not                              */
    ca_classifier_range_t src_port;      /* source port range, start = end for one      */
    ca_classifier_range_t dst_port;      /* destination port range, start = end for one */
    ca_uint16_t           tcp_flags;     /* TCP control bits                            */
} ca_classifier_l4_t;

typedef struct {
    ca_uint32_t l4_valid  : 1;
    ca_uint32_t src_port  : 1;
    ca_uint32_t dst_port  : 1;
    ca_uint32_t tcp_flags : 9;
} ca_classifier_l4_mask_t;

typedef enum {
    CA_CLASSIFIER_FIELD_OFFSET_BASE_PACKET_START,       /*packet first byte                               */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_L2_HEADER,          /* just after Ethernet header                     */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_OUTER_L3_HEADER,    /* just after outer L3 header                     */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_INNER_L3_HEADER,    /* just after inner L3 header                     */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_OUTER_L4_HEADER,    /* just after outer L4 header                     */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_INNER_L4_HEADER,    /* just after inner L4 header                     */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_FIRST_HEADER,       /* just after first header after ethernet header  */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_SECOND_HEADER,      /* just after second header after ethernet header */
    CA_CLASSIFIER_FIELD_OFFSET_BASE_THIRD_START         /* just after third header after ethernet header  */
} ca_classifier_field_offset_base_t;

typedef struct {
    ca_uint16_t	    offset; 		/* offset from start of packet */
    ca_uint16_t	    length; 		/* length of the field, unit is byte*/
    ca_uint8_t	    data[CA_MAX_DATA_EXTRACTION_LEN]; /*data */  
    ca_uint8_t      mask[CA_MAX_DATA_EXTRACTION_LEN]; /*bit mask for data */  
} ca_classifier_ext_t;

typedef struct {
    ca_boolean_t         ip_valid;       /* valid IP check                                   */
    ca_uint32_t          ip_version;     /* IP version, V4 or V6                             */
    ca_uint8_t           ip_protocol;    /* IP protocol, IPv6 in next-header                 */
    ca_uint8_t           dscp;           /* DSCP                                             */
    ca_uint8_t           ecn;            /* ECN                                              */
    ca_ip_address_t      ip_sa;          /* ip_sa source IP address                          */
    ca_ip_address_t      ip_sa_max;      /* range max source IP addr when range is specified */
    ca_ip_address_t      ip_da;          /* ip_da destination IP address                     */
    ca_ip_address_t      ip_da_max;      /* range max dest IP addr when range is specified   */
    ca_uint8_t           fragment ;      /* fragment status : 0-no, 1-first, 2-others        */
    ca_uint32_t          have_options;   /* whether IP-options is inclused or not, IPv4 only */
    ca_uint32_t          flow_label;     /* flow Lable IPv6only                              */
    ca_ipv6_ext_header_t ext_header;     /* IPv6 Extension Headers Type                      */
    ca_uint8_t           icmp_type;      /*ICMP message type                                 */
    ca_uint8_t           igmp_type;      /*IGMP message type                                 */
} ca_classifier_ip_t;

typedef struct {
    ca_uint32_t ip_valid     :1;
    ca_uint32_t ip_version   :1;
    ca_uint32_t ip_protocol  :1;
    ca_uint32_t dscp         :1;
    ca_uint32_t ecn          :1;

    ca_uint32_t ip_sa        :1;  /* enable/disable ip_sa field */
    ca_uint32_t ip_sa_max    :1;  /* 1: range max of ip_sa      */
    ca_uint32_t ip_da        :1;  /* enable/disable ip_da field */
    ca_uint32_t ip_da_max    :1;  /* 1: range max of ip_da      */

    ca_uint32_t fragment     :1;
    ca_uint32_t have_options :1;
    ca_uint32_t flow_label   :1;
    ca_uint32_t ext_header   :1;
    ca_uint32_t icmp_type    :1;
    ca_uint32_t igmp_type    :1;
} ca_classifier_ip_mask_t;


typedef struct {
    ca_port_id_t        orig_src_port; /* for traffic forwarded to L2RP ports */
    ca_port_id_t        src_port;
    ca_port_id_t        dest_port;     /* ACL: dest port based */
    ca_intf_id_t        src_intf;
    ca_intf_id_t        dst_intf;
    ca_classifier_l2_t  l2;       /* layer 2 fields    */
    ca_classifier_ip_t  ip;       /* IP related fields */
    ca_classifier_l4_t  l4;       /* layer 4 fields    */
    ca_classifier_ext_t ext;      /* extended fields   */
} ca_classifier_key_t;

typedef struct {
    ca_uint32_t orig_src_port  : 1;
    ca_uint32_t src_port  : 1;
    ca_uint32_t dest_port : 1;
    ca_uint32_t src_intf  : 1;
    ca_uint32_t dst_intf  : 1;
    ca_uint32_t       l2  : 1;
    ca_uint32_t       ip  : 1;
    ca_uint32_t       l4  : 1;
    ca_uint32_t       ext : 1;
    ca_classifier_l2_mask_t l2_mask; /* valid only when l2 is set */
    ca_classifier_ip_mask_t ip_mask; /* valid only when ip is set */
    ca_classifier_l4_mask_t l4_mask; /* valid only when l4 is set */

} ca_classifier_key_mask_t;

typedef enum {
    CA_CLASSIFIER_FORWARD_DENY,      /* drop packet */
    CA_CLASSIFIER_FORWARD_FE,        /* forward to FE */
    CA_CLASSIFIER_FORWARD_INTERFACE, /* forward to destination interface ID */
    CA_CLASSIFIER_FORWARD_PORT,      /* forward to dpid */
} ca_classifier_forward_flag_t;

typedef enum {
    CA_CLASSIFIER_VLAN_ACTION_NOP,	/* bypass VLAN action */
    CA_CLASSIFIER_VLAN_ACTION_PUSH,   	/* push a VLAN tag */
    CA_CLASSIFIER_VLAN_ACTION_POP,		/* pop the VLAN tag */
    CA_CLASSIFIER_VLAN_ACTION_SWAP,	/* swap the VLAN ID with given */
    CA_CLASSIFIER_VLAN_ACTION_SET		/* set to specified value */
} ca_classifier_vlan_action_t;

typedef struct {
    ca_classifier_vlan_action_t action;    /* VLAN swap is implemented by vid/tpid */

    ca_uint32_t                 tpid; /* TPID        */
    ca_uint32_t                 pri;  /* 802.1p, PCP */
    ca_uint32_t                 dei;  /* CFI or DEI  */
    ca_uint32_t                 vid;  /* VLAN ID     */
} ca_classifier_action_vlan_t;

typedef struct {
    ca_uint32_t action : 1;           /* VLAN swap ignore this bit */
    ca_uint32_t tpid   : 1;
    ca_uint32_t pri    : 1;
    ca_uint32_t dei    : 1;
    ca_uint32_t vid    : 1;
} ca_classifier_action_vlan_mask_t;

typedef struct {
    ca_uint32_t			flow_id	:1;  
    ca_uint32_t			priority	:1;  
    ca_uint32_t			dscp		:1;  
    ca_uint32_t			inner_dot1p	:1;  
    ca_uint32_t			inner_tpid	:1;  
    ca_uint32_t			inner_dei	:1;  
    ca_uint32_t			inner_vlan_act	:1; /* Validate for vlan_act and vid */
    ca_uint32_t			outer_dot1p	:1;  
    ca_uint32_t			outer_tpid	:1;  
    ca_uint32_t			outer_dei	:1;  
    ca_uint32_t			outer_vlan_act	:1; /* Validate for vlan_act and vid */
    ca_uint32_t			mac_da	:1; 
    ca_uint32_t			sw_id	:1;         /* store marker for SW usage */
    ca_uint32_t			sw_shaper_id	:1; /* SW shaper id */
} ca_classifier_action_option_mask_t;


typedef struct {
    ca_uint16_t					flow_id;  	        /* flow ID for following flow based control */
    ca_uint8_t					priority;  		    /* internal priority */
    ca_uint8_t					dscp; 			    /* new dscp value for remark */
    ca_uint16_t					inner_dot1p;		/* inner 802.1P for remark or new VLAN tag */  
    ca_uint16_t					inner_tpid;		    /* inner TPID for remark or new VLAN tag */  
    ca_uint16_t					inner_dei;  		/* inner DEI for Push/Swap VLAN */  
    ca_uint16_t					inner_vid;  		/* inner VLAN ID for Push/Swap VLAN */  
    ca_classifier_vlan_action_t 	inner_vlan_act;	/* inner VLAN action */
    ca_uint16_t					outer_dot1p;		/* outer 802.1P for remark or new VLAN tag */  
    ca_uint16_t					outer_tpid;		    /* outer TPID for remark or new VLAN tag */  
    ca_uint16_t					outer_dei;  		/* outer DEI for Push/Swap VLAN */  
    ca_uint16_t					outer_vid;  		/* outer VLAN ID for Push/Swap VLAN */  
    ca_classifier_vlan_action_t 	outer_vlan_act;	/* outer VLAN action */
    ca_mac_addr_t					mac_da;			/* destination MAC for intf only */
    ca_uint16_t					sw_id[4];		    /* store marker value for SW usage */
    ca_uint16_t					sw_shaper_id;  	    /* SW shaper ID */

    ca_classifier_action_option_mask_t masks;
} ca_classifier_action_option_t;


typedef struct {
    ca_uint32_t	 drop; 	/* not use, reserved for forward = xxx_DENY */
    ca_uint32_t	 fe;    	/* not use, reserved for forward = xxx_FE */
    ca_intf_id_t intf	; 	/* destination for forward = xxx_INTERFACE */	
    ca_port_id_t port; 	/* destination for forward = xxx_PORT */
} ca_classifier_action_dest_t;

typedef struct {
    ca_classifier_forward_flag_t    forward;
    ca_classifier_action_dest_t dest;
    ca_classifier_action_option_t options; /* not valid for drop */
} ca_classifier_action_t;


typedef struct {
    ca_classifier_forward_flag_t    action_type;
    ca_classifier_action_dest_t dest;
} ca_classifier_default_action_t;

typedef struct {
    ca_uint32_t index;
    ca_uint32_t priority;
    ca_classifier_key_t key;
    ca_classifier_key_mask_t key_mask;
    ca_classifier_action_t action;
} ca_classifier_rule_t;

ca_status_t ca_classifier_rule_add (
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              priority,
    CA_IN  ca_classifier_key_t      *key,
    CA_IN  ca_classifier_key_mask_t *key_mask,
    CA_IN  ca_classifier_action_t   *action,
    CA_OUT ca_uint32_t              *index
);

ca_status_t ca_classifier_rule_get (
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_uint32_t              index,
    CA_OUT ca_uint32_t              *priority,
    CA_OUT ca_classifier_key_t      *key,
    CA_OUT ca_classifier_key_mask_t *key_mask,
    CA_OUT ca_classifier_action_t   *action
);

ca_status_t ca_classifier_rule_iterate (
	CA_IN     ca_device_id_t   device_id, 
	CA_OUT ca_iterator_t *p_return_entry
);

ca_status_t ca_classifier_rule_delete (
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32_t index
);

ca_status_t ca_classifier_rule_delete_all (
    CA_IN ca_device_id_t device_id
);

ca_status_t ca_classifier_port_default_action_get (
	CA_IN     ca_device_id_t                    device_id, 
	CA_IN     ca_port_id_t                   port_id,
	CA_IN_OUT ca_classifier_default_action_t *p_action
); 

ca_status_t ca_classifier_port_default_action_set (
	CA_IN ca_device_id_t                    device_id, 
	CA_IN ca_port_id_t                   port_id,
	CA_IN ca_classifier_default_action_t *p_action
); 

#endif /* __CLASSIFIER_H__ */

