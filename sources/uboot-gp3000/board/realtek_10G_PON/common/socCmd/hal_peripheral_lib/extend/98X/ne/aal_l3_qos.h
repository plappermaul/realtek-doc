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
#ifndef __AAL_L3_QOS_H__
#define __AAL_L3_QOS_H__

#include "ca_types.h"

#define AAL_L3_QOS_DOT1P_MAX          7   /* MAX 802.1p priority */
#define AAL_L3_QOS_DSCP_MAX           63  /* MAX DSCP  */
#define AAL_L3_QOS_TC_MAX             63  /* MAX IPv6 Traffic Class support */
#define AAL_L3_QOS_COS_MAX            7   /* MAX internal priority */
#define AAL_L3_QOS_POLICER_ID_MAX     511 /* MAX policer index */
#define AAL_L3_QOS_POLICER_GRP_ID_MAX 7   /* MAX policer group index */
#define AAL_L3_QOS_PROFILE_NUM        2   /* QOS profile number */

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd     :26;
        ca_uint32_t pol_id    :1;
        ca_uint32_t pol_grp_id:1;
        ca_uint32_t cos       :1;
        ca_uint32_t dot1p     :1;
        ca_uint32_t dscp      :1;
        ca_uint32_t tc        :1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t tc        :1;
        ca_uint32_t dscp      :1;
        ca_uint32_t dot1p     :1;
        ca_uint32_t cos       :1;
        ca_uint32_t pol_grp_id:1;
        ca_uint32_t pol_id    :1;
        ca_uint32_t rsrvd     :26;
#endif
    } s;
    ca_uint32_t u32;

} aal_l3_qos_dot1p_map_mask_t;

typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd     :27;
        ca_uint32_t pol_grp_id:1;
        ca_uint32_t pol_id    :1;
        ca_uint32_t cos       :1;
        ca_uint32_t dscp      :1;
        ca_uint32_t dot1p     :1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t dot1p     :1;
        ca_uint32_t dscp      :1;
        ca_uint32_t cos       :1;
        ca_uint32_t pol_id    :1;
        ca_uint32_t pol_grp_id:1;
        ca_uint32_t rsrvd     :27;
#endif
    } s;
    ca_uint32_t u32;

} aal_l3_qos_dscp_map_cfg_mask_t;


typedef union {
    struct {
#ifdef CS_BIG_ENDIAN
        ca_uint32_t rsrvd     :27;
        ca_uint32_t pol_id    :1;
        ca_uint32_t pol_grp_id:1;
        ca_uint32_t dot1p     :1;
        ca_uint32_t dscp      :1;
        ca_uint32_t tc        :1;
#else               /* CS_LITTLE_ENDIAN */
        ca_uint32_t tc        :1;
        ca_uint32_t dscp      :1;
        ca_uint32_t dot1p     :1;
        ca_uint32_t pol_grp_id:1;
        ca_uint32_t pol_id    :1;
        ca_uint32_t rsrvd     :27;
#endif
    } s;
    ca_uint32_t u32;

} aal_l3_qos_cos_profile_mask_t;

typedef struct {
    ca_uint8_t  tc;         /* mapped TC value LSB 6 bits */
    ca_uint8_t  dscp;       /* mapped DSCP value */
    ca_uint8_t  dot1p;      /* mapped 802.1P value */
    ca_uint8_t  cos;        /* mapped internal COS value */
    ca_uint16_t pol_grp_id; /* mapped TE policer group value */
    ca_uint16_t pol_id;     /* mapped TE policer index  */

}aal_l3_qos_dot1p_map_cfg_t;

typedef struct {
    ca_uint8_t  dot1p;      /* mapped 802.1P value */
    ca_uint8_t  dscp;       /* mapped DSCP value */
    ca_uint8_t  cos;        /* mapped internal COS value */
    ca_uint16_t pol_grp_id; /* mapped TE policer group value */
    ca_uint16_t pol_id;     /* mapped TE policer index  */
}aal_l3_qos_dscp_map_cfg_t;

typedef struct {
    ca_uint8_t  tc;        /* mapped TC value LSB 6 bits */
    ca_uint8_t  dscp;      /* mapped DSCP value */
    ca_uint8_t  dot1p;     /* mapped 802.1P value */
    ca_uint16_t pol_grp_id;/* mapped TE policer group value */
    ca_uint16_t pol_id;    /* mapped TE policer index  */
}aal_l3_qos_cos_profile_t;

ca_status_t aal_l3_qos_dscp_map_set(CA_IN ca_device_id_t device_id,
                CA_IN ca_uint8_t dscp,
                CA_IN ca_uint8_t profile_idx,
                CA_IN aal_l3_qos_dscp_map_cfg_mask_t mask,
                CA_IN aal_l3_qos_dscp_map_cfg_t * config);

ca_status_t aal_l3_qos_dscp_map_get(CA_IN ca_device_id_t device_id,
                CA_IN ca_uint8_t dscp,
                CA_IN ca_uint8_t profile_idx,
                CA_OUT aal_l3_qos_dscp_map_cfg_t * config);

ca_status_t aal_l3_qos_tc_map_set(CA_IN ca_device_id_t device_id,
                CA_IN ca_uint8_t tc,
                CA_IN ca_uint8_t profile_idx,
                CA_IN aal_l3_qos_dscp_map_cfg_mask_t mask,
                CA_IN aal_l3_qos_dscp_map_cfg_t * config);

ca_status_t aal_l3_qos_tc_map_get(CA_IN ca_device_id_t device_id,
                 CA_IN ca_uint8_t tc,
                 CA_IN ca_uint8_t profile_idx,
                 CA_OUT aal_l3_qos_dscp_map_cfg_t * config);

ca_status_t aal_l3_qos_dot1p_map_set(CA_IN ca_device_id_t device_id,
                CA_IN ca_uint8_t dot1p,
                CA_IN ca_uint8_t profile_idx,
                CA_IN aal_l3_qos_dot1p_map_mask_t mask,
                CA_IN aal_l3_qos_dot1p_map_cfg_t * config);

ca_status_t aal_l3_qos_dot1p_map_get(CA_IN ca_device_id_t device_id,
                CA_IN ca_uint8_t dot1p,
                CA_IN ca_uint8_t profile_idx,
                CA_OUT aal_l3_qos_dot1p_map_cfg_t * config);

ca_status_t aal_l3_qos_cos_map_set(CA_IN ca_device_id_t device_id,
                CA_IN ca_uint8_t cos,
                CA_IN ca_uint8_t profile_idx,
                CA_IN aal_l3_qos_cos_profile_mask_t mask,
                CA_IN aal_l3_qos_cos_profile_t * config);

ca_status_t aal_l3_qos_cos_map_get(
                CA_IN  ca_device_id_t device_id,
                CA_IN  ca_uint8_t cos,
                CA_IN  ca_uint8_t profile_idx,
                CA_OUT aal_l3_qos_cos_profile_t * config);

ca_status_t aal_l3_qos_init(/* AUTO-CLI gen ignore */
                CA_IN ca_device_id_t device_id);

#endif

