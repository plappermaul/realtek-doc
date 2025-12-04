#ifndef __QOS_H__
#define __QOS_H__

#include "ca_types.h"

// internal priority (CoS) mapping options
typedef enum {
    /* tagged: tagged: use 802.1P => CoS mapping table; untagged: use
       default priority. Mapping table is setup with ca_qos_dot1p_map_set
     */
    CA_QOS_MAP_DOT1P_MODE = 0,

    /* TC =>Cos mapping table for IPv6 packts  DSCP=>CoS mapping
       for IPv4 packets, non-IP, use  default priority
       mapping table setup with ca_qos_dscp/tc_map_set
     */
    CA_QOS_MAP_DSCP_TC_MODE = 1,

    /* CoS mapped from classifier action, CoS value set by
       classifier action or ca_l2_vlan_ingress_action_add,
       ca_l2_vlan_ingress_default_action_set, or ca_classifier_rule_add
     */
    CA_QOS_MAP_CLASSIFIER = 2
} ca_qos_map_mode_t;

// TX 801.21P remap options
typedef enum {
    /* tagged: use 802.1P => new 802.1P mapping table; untagged:
       Use default 1P value. Mapping table is setup with ca_qos_dot1p_remark
     */
    CA_DOT1P_MAP_DOT1P_MODE = 0,

    /* TC =>1P mapping table for IPv6 packts  DSCP=>1P mapping
       for IPv4 packets, non-IP, use  default 1P value
       mapping table setup with ca_qos_dscp/tc_dot1p_translate_set
     */
    CA_DOT1P_MAP_DSCP_TC_MODE = 1,

    /* 1P value set by classifier action or ca_l2_vlan_egress/ingress_action_add,
       ca_l2_vlan_egress/ingress_default_action_set, or ca_classifier_rule_add
     */
    CA_DOT1P_MAP_CLASSIFIER = 2
} ca_1p_map_mode_t;

// TX DSCP/TC remap options
typedef enum {
    /* TC =>TC mapping table for IPv6 packts  DSCP=>DSCP mapping
       for IPv4 packets, non-IP, use  default DSCP
       mapping table setup with ca_qos_dscp/tc_remark_set
    */
    CA_DSCP_MAP_DSCP_TC_MODE = 1,

    /* DSCP/TC mapped from classifier action, DSCP/TC value
       set by classifier action or ca_l2_vlan_egress/ingress_action_add,
       ca_l2_vlan_egress/ingress_default_action_set, ca_classifier_rule_add
    */
    CA_DSCP_MAP_CLASSIFIER = 2
} ca_dscp_map_mode_t;

typedef struct {
    ca_qos_map_mode_t  qos_map_mode;     /* refer to ca_qos_map_mode_t        */
    ca_1p_map_mode_t   dot1p_remap_mode; /* refer to ca_1p_map_mode_t */
    ca_dscp_map_mode_t dscp_remap_mode;  /* refer to ca_dscp_map_mode_t */
} ca_qos_config_t;

ca_status_t ca_qos_config_set(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_IN ca_qos_config_t * config);

ca_status_t ca_qos_config_get(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_OUT ca_qos_config_t * config);

ca_status_t ca_qos_dot1p_map_set(CA_IN ca_device_id_t device_id,
                 CA_IN ca_port_id_t port_id,
                 CA_IN ca_uint8_t dot1p,
                 CA_IN ca_uint8_t priority);

ca_status_t ca_qos_dot1p_map_get(CA_IN ca_device_id_t device_id,
                 CA_IN ca_port_id_t port_id,
                 CA_IN ca_uint8_t dot1p,
                 CA_OUT ca_uint8_t * priority);

ca_status_t ca_qos_dot1p_remark_set(CA_IN ca_device_id_t device_id,
                    CA_IN ca_port_id_t port_id,
                    CA_IN ca_uint8_t old_dot1p,
                    CA_IN ca_uint8_t new_dot1p);

ca_status_t ca_qos_dot1p_remark_get(CA_IN ca_device_id_t device_id,
                    CA_IN ca_port_id_t port_id,
                    CA_IN ca_uint8_t old_dot1p,
                    CA_OUT ca_uint8_t * new_dot1p);

ca_status_t ca_qos_dscp_map_set(CA_IN ca_device_id_t device_id,
                CA_IN ca_port_id_t port_id,
                CA_IN ca_uint8_t dscp,
                CA_IN ca_uint8_t priority);

ca_status_t ca_qos_dscp_map_get(CA_IN ca_device_id_t device_id,
                CA_IN ca_port_id_t port_id,
                CA_IN ca_uint8_t dscp,
                CA_OUT ca_uint8_t * priority);

ca_status_t ca_qos_dscp_remark_set(CA_IN ca_device_id_t device_id,
                   CA_IN ca_port_id_t port_id,
                   CA_IN ca_uint8_t old_dscp,
                   CA_IN ca_uint8_t new_dscp);

ca_status_t ca_qos_dscp_remark_get(CA_IN ca_device_id_t device_id,
                   CA_IN ca_port_id_t port_id,
                   CA_IN ca_uint8_t old_dscp,
                   CA_OUT ca_uint8_t * new_dscp);

ca_status_t ca_qos_dscp_dot1p_translate_set(CA_IN ca_device_id_t device_id,
                        CA_IN ca_port_id_t port_id,
                        CA_IN ca_uint8_t dscp,
                        CA_IN ca_uint8_t dot1p);

ca_status_t ca_qos_dscp_dot1p_translate_get(CA_IN ca_device_id_t device_id,
                        CA_IN ca_port_id_t port_id,
                        CA_IN ca_uint8_t dscp,
                        CA_OUT ca_uint8_t * dot1p);

ca_status_t ca_qos_tc_map_set(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_IN ca_uint8_t tc, CA_IN ca_uint8_t priority);

ca_status_t ca_qos_tc_map_get(CA_IN ca_device_id_t device_id,
                  CA_IN ca_port_id_t port_id,
                  CA_IN ca_uint8_t tc,
                  CA_OUT ca_uint8_t * priority);

ca_status_t ca_qos_tc_remark_set(CA_IN ca_device_id_t device_id,
                 CA_IN ca_port_id_t port_id,
                 CA_IN ca_uint8_t old_tc,
                 CA_IN ca_uint8_t new_tc);

ca_status_t ca_qos_tc_remark_get(CA_IN ca_device_id_t device_id,
                 CA_IN ca_port_id_t port_id,
                 CA_IN ca_uint8_t old_tc,
                 CA_OUT ca_uint8_t * new_tc);

ca_status_t ca_qos_tc_dot1p_translate_set(CA_IN ca_device_id_t device_id,
                      CA_IN ca_port_id_t port_id,
                      CA_IN ca_uint8_t tc,
                      CA_IN ca_uint8_t dot1p);

ca_status_t ca_qos_tc_dot1p_translate_get(CA_IN ca_device_id_t device_id,
                      CA_IN ca_port_id_t port_id,
                      CA_IN ca_uint8_t tc,
                      CA_OUT ca_uint8_t * dot1p);

#endif
