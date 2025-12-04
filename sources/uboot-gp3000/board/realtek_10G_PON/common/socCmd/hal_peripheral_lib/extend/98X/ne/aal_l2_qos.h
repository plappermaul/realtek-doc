/*
 * Copyright (c) Cortina-Access Limited 2015.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __AAL_L2_QOS_H__
#define __AAL_L2_QOS_H__

#include "ca_types.h"

#define AAL_L2_QOS_DOT1P_MAX       7    /* MAX 802.1p priority */
#define AAL_L2_QOS_DSCP_MAX        63   /* MAX DSCP  */
#define AAL_L2_QOS_TC_MAX          255  /* MAX IPv6 Traffic Class support here */
#define AAL_L2_QOS_PORT_MAX        63   /* MAX Internal Port  */
#define AAL_L2_QOS_COS_MAX         7    /* MAX internal priority */

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd:28;
        ca_uint32_t dscp_map_mapped_dscp:1;
        ca_uint32_t dscp_map_marked_flag:1;
        ca_uint32_t dscp_map_mapped_cos:1;
        ca_uint32_t mapped_premark:1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t mapped_premark:1;
        ca_uint32_t dscp_map_mapped_cos:1;
        ca_uint32_t dscp_map_marked_flag:1;
        ca_uint32_t dscp_map_mapped_dscp:1;
        ca_uint32_t rsrvd:28;
#endif
    } s;
    ca_uint32_t u32;
} aal_l2_qos_dscp_map_mask_t;

typedef struct {
    ca_boolean_t mapped_premark;/*mapped color for traffic is premarked or not*/
    ca_boolean_t dscp_map_marked_flag; /* DSCP is already marked or not  */
    ca_uint8_t   dscp_map_mapped_cos; /* mapped COS for this DSCP */
    ca_uint8_t   dscp_map_mapped_dscp;/* mapped DSCP for this DSCP */
} aal_l2_qos_dscp_map_t;

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd:28;
        ca_uint32_t tc_map_mapped_dscp:1;
        ca_uint32_t tc_map_marked_flag:1;
        ca_uint32_t tc_map_mapped_cos:1;
        ca_uint32_t mapped_premark:1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t mapped_premark:1;
        ca_uint32_t tc_map_mapped_cos:1;
        ca_uint32_t tc_map_marked_flag:1;
        ca_uint32_t tc_map_mapped_dscp:1;
        ca_uint32_t rsrvd:28;
#endif
    } s;
    ca_uint32_t u32;
} aal_l2_qos_tc_map_mask_t;

typedef struct {
    ca_boolean_t mapped_premark;/*mapped color for traffic is premarked or not*/
    ca_boolean_t tc_map_marked_flag; /* DSCP is already marked or not  */
    ca_uint8_t   tc_map_mapped_cos; /* mapped COS for this DSCP */
    ca_uint8_t   tc_map_mapped_dscp;/* mapped DSCP for this DSCP */
} aal_l2_qos_tc_map_t;


typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd:29;
        ca_uint32_t dot1p_map_mapped_1p:1;
        ca_uint32_t dot1p_map_mapped_cos:1;
        ca_uint32_t dot1p_map_mapped_premark:1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t dot1p_map_mapped_premark:1;
        ca_uint32_t dot1p_map_mapped_cos:1;
        ca_uint32_t dot1p_map_mapped_1p:1;
        ca_uint32_t rsrvd:29;
#endif
    } s;
    ca_uint32_t u32;
} aal_l2_qos_dot1p_map_mask_t;

typedef struct {
    ca_boolean_t dot1p_map_mapped_premark;
    ca_uint8_t   dot1p_map_mapped_cos;
    ca_uint8_t   dot1p_map_mapped_1p;
} aal_l2_qos_dot1p_map_t;

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd:30;
        ca_uint32_t txdscp_map_mapped_1p:1;
        ca_uint32_t txdscp_map_mapped_cos:1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t txdscp_map_mapped_cos:1;
        ca_uint32_t txdscp_map_mapped_1p:1;
        ca_uint32_t rsrvd:30;
#endif
    } s;

    ca_uint32_t u32;
} aal_l2_qos_txdscp_map_mask_t;

typedef struct {
    ca_uint8_t txdscp_map_mapped_cos; /* egress mapped COS value  */
    ca_uint8_t txdscp_map_mapped_1p;  /* egress mapped 802.1p value*/
} aal_l2_qos_txdscp_map_t;

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd:30;
        ca_uint32_t txtc_map_mapped_1p:1;
        ca_uint32_t txtc_map_mapped_cos:1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t txtc_map_mapped_cos:1;
        ca_uint32_t txtc_map_mapped_1p:1;
        ca_uint32_t rsrvd:30;
#endif
    } s;
    ca_uint32_t u32;
} aal_l2_qos_txtc_map_mask_t;

typedef struct {
    ca_uint8_t txtc_map_mapped_cos; /* egress mapped COS value  */
    ca_uint8_t txtc_map_mapped_1p;  /* egress mapped 802.1p value*/

} aal_l2_qos_txtc_map_t;

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd:31;
        ca_uint32_t txdot1p_map_mapped_cos:1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t txdot1p_map_mapped_cos:1;
        ca_uint32_t rsrvd:31;
#endif
    } s;
    ca_uint32_t u32;
} aal_l2_qos_txdot1p_map_mask_t;

typedef struct {
    ca_uint8_t txdot1p_map_mapped_cos;
} aal_l2_qos_txdot1p_map_t;

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN

        ca_uint32_t rsrvd:29;
        ca_uint32_t dot1p_mode_sel:1;
        ca_uint32_t dscp_mode_sel:1;
        ca_uint32_t cos_mode_sel:1;

#else               /* CS_LITTLE_ENDIAN */

        ca_uint32_t rsrvd:29;
        ca_uint32_t cos_mode_sel:1;
        ca_uint32_t dscp_mode_sel:1;
        ca_uint32_t dot1p_mode_sel:1;

#endif
    } s;
    ca_uint32_t u32;
} aal_l2_qos_port_config_mask_t;

#define AAL_COS_MODE_MAP_RX_TOP_DOT1P  0 /* Final COS <= map from Rx_Top_8021p; */
#define AAL_COS_MODE_MAP_RX_DSCP       1 /* Final COS <= map from Rx_ DSCP (If non-IP; map Rx_Top_8021p);  */
#define AAL_COS_MODE_COS_SEL_BMP       2 /* (Marking): Final COS <= ILPB_COS_Select_BM;  */
#define AAL_COS_MODE_PORT_DEF_COS      3 /* From ILPB.COS; */
#define AAL_COS_MODE_MAP_TX_TOP_DOT1P  4 /* Final COS <= map from Tx_Top_8021p;  */
#define AAL_COS_MODE_MAP_TX_DSCP       5 /* Final COS <= map from Tx_ DSCP (If non-IP; map Tx_Top_8021p);  */

#define AAL_DOT1P_MODE_RX_DOT1P_OR_PORT_DEF  0 /* Outgoing top 8021p <= Rx_Top_8021p or ILPB default 8021p(Untagged) */
#define AAL_DOT1P_MODE_DOT1P_SEL_BMP         1 /* Outgoing top 8021p <= ILPB_8021p_Mark_BM */
#define AAL_DOT1P_MODE_MAP_RX_DOT1P          2 /* Outgoing top 8021p <= Map from Received top 8021p */
#define AAL_DOT1P_MODE_MAP_TX_DSCP           3 /* Outgoing top 8021p <= Map from Outgoing DSCP */

#define AAL_DSCP_MODE_RX_DSCP       0 /* Final DSCP <= Rx_DSCP */
#define AAL_DSCP_MODE_DSCP_SEL_BMP  1 /* Final DSCP <= ILPB_DSCP_Mark_BM */
#define AAL_DSCP_MODE_MAP_RX_DSCP   2 /* Final DSCP <= Map from Rx DSCP */
#define AAL_DSCP_MODE_DEF_DSCP      3 /* Final DSCP <= ILPB default DSCP */

typedef struct {
    ca_uint32_t cos_mode_sel;  /* COS mode selection:
                                0 - map from Rx_Top_8021p.
                                1 - map from Rx_DSCP (If non-IP map Rx_Top_8021p).
                                2 - ILPB_COS_Select_BM.
                                3 - From ILPB.COS.
                                4 - map from Tx_Top_8021p.
                                5 - map from Tx_ DSCP (If non-IP; map Tx_Top_8021p).
                                6,7 - same as 0 */
    ca_uint32_t dscp_mode_sel; /* DSCP mode selection:
                                0 - Rx_DSCP. 
                                1 - ILPB_DSCP_Mark_BM.
                                2 - Map from Rx DSCP.
                                3 - from ILPB default DSCP. */

    ca_uint32_t dot1p_mode_sel;/* Dot1P mode selection: 
                                  0 - Rx_Top_8021p or ILPB default 8021p fo Untagged packets
                                  1 - ILPB_8021p_Mark_BM
                                  2 - Map from Received top 8021p
                                  3 - Map from Outgoing DSCP */
} aal_l2_qos_port_config_t;

typedef enum {
    AAL_L2_QOS_DIR_UPSTREAM   = 0, /* upstream or LAN to LAN */
    AAL_L2_QOS_DIR_DOWNSTREAM = 1 /* downstream */
} aal_l2_qos_direction_t;

ca_status_t aal_l2_qos_dscp_map_set(CA_IN ca_device_id_t device_id,
                 CA_IN aal_l2_qos_direction_t direction,
                 CA_IN ca_uint32_t dscp,
                 CA_IN aal_l2_qos_dscp_map_mask_t mask,
                 CA_IN aal_l2_qos_dscp_map_t * config);

ca_status_t aal_l2_qos_dscp_map_get(CA_IN ca_device_id_t device_id,
                 CA_IN aal_l2_qos_direction_t direction,
                 CA_IN ca_uint32_t dscp,
                 CA_OUT aal_l2_qos_dscp_map_t * config);
                   
ca_status_t aal_l2_qos_tc_map_set(CA_IN ca_device_id_t device_id,
                 CA_IN aal_l2_qos_direction_t direction,
                 CA_IN ca_uint32_t tc,
                 CA_IN aal_l2_qos_tc_map_mask_t mask,
                 CA_IN aal_l2_qos_tc_map_t * config);

ca_status_t aal_l2_qos_tc_map_get(CA_IN ca_device_id_t device_id,
                 CA_IN aal_l2_qos_direction_t direction,
                 CA_IN ca_uint32_t tc,
                 CA_OUT aal_l2_qos_tc_map_t * config);

ca_status_t aal_l2_qos_dot1p_map_set(CA_IN ca_device_id_t device_id,
                  CA_IN aal_l2_qos_direction_t direction,
                  CA_IN ca_uint32_t dot1p,
                  CA_IN aal_l2_qos_dot1p_map_mask_t mask,
                  CA_IN aal_l2_qos_dot1p_map_t * config);

ca_status_t aal_l2_qos_dot1p_map_get(CA_IN ca_device_id_t device_id,
                  CA_IN  aal_l2_qos_direction_t direction,
                  CA_IN  ca_uint32_t dot1p,
                  CA_OUT aal_l2_qos_dot1p_map_t * config);

ca_status_t aal_l2_qos_txdscp_map_set(CA_IN ca_device_id_t device_id,
                   CA_IN aal_l2_qos_direction_t direction,
                   CA_IN ca_uint32_t dscp,
                   CA_IN aal_l2_qos_txdscp_map_mask_t mask,
                   CA_IN aal_l2_qos_txdscp_map_t * config);

ca_status_t aal_l2_qos_txdscp_map_get(CA_IN ca_device_id_t device_id,
                   CA_IN aal_l2_qos_direction_t direction,
                   CA_IN ca_uint32_t dscp,
                   CA_OUT aal_l2_qos_txdscp_map_t * config);

ca_status_t aal_l2_qos_txdot1p_map_set(CA_IN ca_device_id_t device_id,
                    CA_IN aal_l2_qos_direction_t direction,
                    CA_IN ca_uint32_t dot1p,
                    CA_IN aal_l2_qos_txdot1p_map_mask_t mask,
                    CA_IN aal_l2_qos_txdot1p_map_t * config);

ca_status_t aal_l2_qos_txdot1p_map_get(CA_IN ca_device_id_t device_id,
                    CA_IN aal_l2_qos_direction_t direction,
                    CA_IN ca_uint32_t dot1p,
                    CA_OUT aal_l2_qos_txdot1p_map_t * config);

ca_status_t aal_l2_qos_txtc_map_set(CA_IN ca_device_id_t device_id,
                 CA_IN aal_l2_qos_direction_t direction,
                 CA_IN ca_uint32_t tc,
                 CA_IN aal_l2_qos_txtc_map_mask_t mask,
                 CA_IN aal_l2_qos_txtc_map_t * config);

ca_status_t aal_l2_qos_txtc_map_get(CA_IN ca_device_id_t device_id,
                 CA_IN aal_l2_qos_direction_t direction,
                 CA_IN ca_uint32_t tc,
                 CA_OUT aal_l2_qos_txtc_map_t * config);

ca_status_t aal_l2_qos_port_config_set(CA_IN ca_device_id_t device_id,
                    CA_IN ca_port_id_t port_id,
                    CA_IN aal_l2_qos_port_config_mask_t mask,
                    CA_IN aal_l2_qos_port_config_t * config);

ca_status_t aal_l2_qos_port_config_get(CA_IN ca_device_id_t device_id,
                    CA_IN ca_port_id_t port_id,
                    CA_OUT aal_l2_qos_port_config_t * config);

ca_status_t aal_l2_qos_init(/* not support auto-CLI */
    CA_IN ca_device_id_t device_id);

#endif

