#ifndef __PORT_H__
#define __PORT_H__

#include "ca_types.h"
#include "classifier.h"

#define CA_PORT_INDEX_INVALID 			0xFF
#define CA_PORT_INDEX_SHIFT 			0
#define CA_PORT_INDEX_MASK  			0xFF

typedef enum {/*CODEGEN_IGNORE_TAG*/
	CA_PORT_ID_NI0 = 0,     /* 1 Gbps NI Port (Integrated GPHY) */
	CA_PORT_ID_NI1 = 1,     /* 1 Gbps NI Port (Integrated GPHY) */
	CA_PORT_ID_NI2 = 2,     /* 1 Gbps NI Port (Integrated GPHY) */
	CA_PORT_ID_NI3 = 3,     /* 1 Gbps NI Port (Integrated GPHY) */	
	CA_PORT_ID_NI4 = 4,     /* 1 Gbps NI Port (RGMII)*/
#ifndef CONFIG_ARCH_CORTINA_G3LITE
	CA_PORT_ID_NI5 = 5,     /* 10 Gbps XFI NI Port (RXAUI/2.5GSGMII/RGMII) */
	CA_PORT_ID_NI6 = 6,     /* 10 Gbps XFI NI Port (LAN) */
	CA_PORT_ID_NI7 = 7,     /* 10 Gbps XFI NI Port (WAN) */
#endif/*CODEGEN_IGNORE_TAG*/
#ifdef CONFIG_ARCH_CORTINA_G3LITE/*CODEGEN_IGNORE_TAG*/
	CA_PORT_ID_NI5 = CA_PORT_INDEX_INVALID,     
	CA_PORT_ID_NI6 = CA_PORT_INDEX_INVALID,     
	CA_PORT_ID_NI7 = CA_PORT_INDEX_INVALID,     
#endif/*CODEGEN_IGNORE_TAG*/
	CA_PORT_ID_DEEPQ0 = 0,
	CA_PORT_ID_DEEPQ1 = 1,
	CA_PORT_ID_DEEPQ2 = 2,
	CA_PORT_ID_DEEPQ3 = 3,
	CA_PORT_ID_DEEPQ4 = 4,
#ifndef CONFIG_ARCH_CORTINA_G3LITE
	CA_PORT_ID_DEEPQ5 = 5,
	CA_PORT_ID_DEEPQ6 = 6,
	CA_PORT_ID_DEEPQ7 = 7,
#endif/*CODEGEN_IGNORE_TAG*/
#ifdef CONFIG_ARCH_CORTINA_G3LITE/*CODEGEN_IGNORE_TAG*/
	CA_PORT_ID_DEEPQ5 = CA_PORT_INDEX_INVALID,
	CA_PORT_ID_DEEPQ6 = CA_PORT_INDEX_INVALID,
	CA_PORT_ID_DEEPQ7 = CA_PORT_INDEX_INVALID,
#endif/*CODEGEN_IGNORE_TAG*/
	CA_PORT_ID_CPU0 = 16,
	CA_PORT_ID_CPU1 = 17,
	CA_PORT_ID_CPU2 = 18,
	CA_PORT_ID_CPU3 = 19,
	CA_PORT_ID_CPU4 = 20,
	CA_PORT_ID_CPU5 = 21,
	CA_PORT_ID_CPU6 = 22,
	CA_PORT_ID_CPU7 = 23,

	CA_PORT_ID_L3_WAN = 24,	    /* port on L3 (interface) */
	CA_PORT_ID_L3_LAN = 25,	    /* port on L3 (interface) */

	CA_PORT_ID_OAM = 26,        /* oam port */
	CA_PORT_ID_MC = 27,         /* multicast port */
	CA_PORT_ID_CPU_Q = 29,      /* cpu port */
	CA_PORT_ID_SPCL = 30,       /* ldpid for special packet resolution */
	CA_PORT_ID_BLACKHOLE = 31,

	CA_PORT_ID_CPU_MQ_MIN = 32,
	CA_PORT_ID_CPU_MQ_MAX = 63,

} ca_port_index_t;

#ifdef CONFIG_ARCH_CORTINA_G3LITE/*CODEGEN_IGNORE_TAG*/
#define CA_PORT_MAX_NUM_ETHERNET	 	5
#endif

#ifdef CONFIG_ARCH_CORTINA_SATURN_SFU/*CODEGEN_IGNORE_TAG*/
#define CA_PORT_MAX_NUM_ETHERNET	 	4
#endif

#ifdef CONFIG_ARCH_CORTINA_G3

#ifndef CONFIG_CORTINA_BOARD_FPGA/*CODEGEN_IGNORE_TAG*/
#define CA_PORT_MAX_NUM_ETHERNET	 	8
#else /*CODEGEN_IGNORE_TAG*/
#define CA_PORT_MAX_NUM_ETHERNET	 	4
#endif/*CODEGEN_IGNORE_TAG*/

#endif


#define CA_PORT_MAX_NUM_L2RP			2	/* L3_LAN, L3_WAN */
#define CA_PORT_MAX_NUM_CPU			43
#define CA_PORT_MAX_NUM_TRUNK			2
#define CA_PORT_MAX_NUM_MC			1

#define CA_PORT_MAX_NUM				64

#define CA_PORT_MAX_NUM_TRUNK_MEMBER	16

/******************************************************************************************************
* Port type check macros
*
******************************************************************************************************/
#define CA_PORT_VALID_TEST(/*CODEGEN_IGNORE_TAG*/_port, _port_type, min, max)     ((PORT_TYPE(_port)==_port_type) && (PORT_ID(_port) >= min) && (PORT_ID(_port) <= max))

#define CA_PORT_IS_ETHERNET(/*CODEGEN_IGNORE_TAG*/_port)     		CA_PORT_VALID_TEST(_port, CA_PORT_TYPE_ETHERNET, CA_PORT_ID_NI0, CA_PORT_ID_NI7)

#define CA_PORT_IS_CPU(/*CODEGEN_IGNORE_TAG*/_port)   			CA_PORT_VALID_TEST(_port, CA_PORT_TYPE_CPU, CA_PORT_ID_CPU0, CA_PORT_ID_CPU7)

#define CA_PORT_IS_L2RP(/*CODEGEN_IGNORE_TAG*/_port)				CA_PORT_VALID_TEST(_port, CA_PORT_TYPE_L2RP, CA_PORT_ID_L3_WAN, CA_PORT_ID_L3_LAN)

#define CA_PORT_ETHERNET_CHECK(/*CODEGEN_IGNORE_TAG*/port)    	if (!CA_PORT_IS_ETHERNET(port)) { return CA_E_PARAM;}
#define CA_PORT_CPU_CHECK(/*CODEGEN_IGNORE_TAG*/port)   		if (!CA_PORT_IS_CPU(port)) { return CA_E_PARAM;}
#define CA_PORT_L2RP_CHECK(/*CODEGEN_IGNORE_TAG*/port)    		if (!CA_PORT_IS_L2RP(port)) { return CA_E_PARAM;}

#define CA_PORT_ID_CHECK(/*CODEGEN_IGNORE_TAG*/port)   			if (!CA_PORT_IS_ETHERNET(port) && !CA_PORT_IS_CPU(port) && !CA_PORT_IS_L2RP(port)) { return CA_E_PARAM;}

#define CA_PORT_LAG_ID_CHECK(/*CODEGEN_IGNORE_TAG*/ gid)          if((gid) > CA_PORT_MAX_NUM_TRUNK-1) {return CA_E_PARAM;}

#define CA_PORT_IPG_GE_VAL_MIN             8
#define CA_PORT_IPG_GE_VAL_MAX             15
#define CA_PORT_IPG_XAUI_VAL_MIN           5
#define CA_PORT_IPG_XAUI_VAL_MAX           0xffffffff
#define CA_PORT_IPG_XGE_VAL_MIN            5
#define CA_PORT_IPG_XGE_VAL_MAX            0xffff

#define CA_PORT_VLAN_TAG_MAX 2

typedef enum {
	CA_PORT_ENCAP_INTERNAL = 0,
	CA_PORT_ENCAP_ETH_E = 1,
	CA_PORT_ENCAP_ETH_1Q_E = 2,
	CA_PORT_ENCAP_ETH_QinQ_E = 3,
} ca_port_encap_type_t;

typedef struct {
	ca_port_encap_type_t type;
	ca_mac_addr_t src_mac;
	ca_uint32_t tag[CA_PORT_VLAN_TAG_MAX];
} ca_port_encap_t;

typedef enum {
	CA_PORT_STP_DISABLED = 0,
	CA_PORT_STP_BLOCKING = 1,
	CA_PORT_STP_LEARNING = 2,
	CA_PORT_STP_FORWARDING = 3,
} ca_port_stp_state_t;

typedef enum {
	CA_PORT_LOOPBACK_NONE = 0,
	CA_PORT_LOOPBACK_MAC_LOCAL = 1,
	CA_PORT_LOOPBACK_MAC_REMOTE = 2,
	CA_PORT_LOOPBACK_PHY_LOCAL = 3,
	CA_PORT_LOOPBACK_PHY_REMOTE = 4
} ca_port_loopback_mode_t;

typedef struct {
	ca_boolean_t drop_untag;	/* drop or permit the untagged packets coming from this port_id. {FALSE, TRUE}         */
	ca_boolean_t drop_pritag;	/* drop or permit the priority tagged packets coming from this port_id.{FALSE, TRUE}   */
	ca_boolean_t drop_mtag;	    /* drop or permit multiple(>=2) tagged packets coming from this port_id. {FALSE, TRUE} */
	ca_boolean_t drop_stag;	    /* drop or permit the tagged packets(one tag) coming from this port_id. {FALSE, TRUE}  */
} ca_port_discard_control_t;


#define CA_PORT_LOGIC_INDEX_SHIFT 0
#define CA_PORT_LOGIC_INDEX_MASK  0xFF


ca_status_t ca_port_enable_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_boolean_t enable);

ca_status_t ca_port_enable_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_boolean_t * enable);

ca_status_t ca_port_max_frame_size_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_uint32_t size);

ca_status_t ca_port_max_frame_size_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * size);

ca_status_t ca_port_ipg_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id, CA_IN ca_uint32_t ipg);

ca_status_t ca_port_ipg_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * ipg);

ca_status_t ca_port_untagged_priority_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_uint32_t priority);

ca_status_t ca_port_untagged_priority_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * priority);

ca_status_t ca_port_tpid_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_uint32_t tpid);

ca_status_t ca_port_tpid_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * tpid);

ca_status_t ca_port_stp_state_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_port_stp_state_t stp_state);

ca_status_t ca_port_stp_state_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_port_stp_state_t * stp_state);

ca_status_t ca_port_isolation_set(CA_IN ca_device_id_t device_id, 
				CA_IN ca_port_id_t port_id, 
				CA_IN ca_uint8_t port_count, 
				CA_IN ca_port_id_t *dst_ports /*[port_count:8] */);

ca_status_t ca_port_isolation_get(CA_IN ca_device_id_t device_id, 
				CA_IN ca_uint32_t port_id, 
				CA_OUT ca_uint8_t * port_count, 
				CA_OUT ca_port_id_t * dst_ports  /*[*port_count:8] */);

ca_status_t ca_port_discard_control_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_port_discard_control_t *control);

ca_status_t ca_port_discard_control_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_port_discard_control_t * control);

ca_status_t ca_port_station_move_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_boolean_t enable);

ca_status_t ca_port_station_move_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_boolean_t *enable);

ca_status_t ca_port_loopback_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_port_loopback_mode_t loopback);

ca_status_t ca_port_loopback_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_port_loopback_mode_t * loopback);

ca_status_t ca_port_pvid_vlan_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_uint32_t vid);

ca_status_t ca_port_pvid_vlan_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * vid);

ca_status_t ca_port_pvid_tpid_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_uint32_t tpid_index);

ca_status_t ca_port_pvid_tpid_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * tpid_index);

ca_status_t ca_port_pvid_dot1p_set(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_IN ca_uint32_t priority);

ca_status_t ca_port_pvid_dot1p_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_port_id_t port_id,
				CA_OUT ca_uint32_t * priority);

ca_status_t ca_port_link_aggregation_group_create(CA_IN ca_device_id_t device_id,
				CA_IN ca_uint8_t lag_group_id,
				CA_IN ca_uint8_t member_port_count,
				CA_IN ca_port_id_t *member_ports,/*[member_port_count:8] */
				CA_IN ca_classifier_key_mask_t *lag_hash_keys);

ca_status_t ca_port_link_aggregation_group_update(CA_IN ca_device_id_t device_id,
				CA_IN ca_uint8_t lag_group_id,
				CA_IN ca_uint8_t member_port_count,
				CA_IN ca_port_id_t *member_ports,/*[member_port_count:8] */
				CA_IN ca_classifier_key_mask_t *lag_hash_keys);

ca_status_t ca_port_link_aggregation_group_get(CA_IN ca_device_id_t device_id,
				CA_IN ca_uint8_t lag_group_id,
				CA_OUT ca_uint8_t * member_port_count,
				CA_OUT ca_port_id_t *member_ports,/*[*member_port_count:8] */
				CA_OUT ca_classifier_key_mask_t * lag_hash_keys);

ca_status_t ca_port_link_aggregation_group_delete(CA_IN ca_device_id_t device_id,
				CA_IN ca_uint8_t lag_group_id);

ca_status_t ca_port_tx_traffic_block_get(CA_IN ca_device_id_t device_id,
                                         CA_IN ca_port_id_t port_id,
                                         CA_OUT ca_boolean_t  *enable
                                         );
ca_status_t ca_port_tx_traffic_block_set(CA_IN ca_device_id_t device_id,
                                         CA_IN ca_port_id_t port_id,
                                         CA_IN ca_boolean_t enable
                                         );

ca_status_t ca_port_egr_qm_block_set(CA_IN ca_device_id_t device_id,
                                                 CA_IN ca_port_id_t port_id,
                                                 CA_IN ca_boolean_t  enable
                                            );
ca_status_t ca_port_egr_qm_block_get(CA_IN ca_device_id_t device_id,
                                         CA_IN ca_port_id_t port_id,
                                         CA_OUT ca_boolean_t  *enable
                                         );

#endif
