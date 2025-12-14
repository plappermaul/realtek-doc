#ifndef __RATE_H__
#define __RATE_H__

#include "ca_types.h"

#define CA_PILICER_L3_FLOW_ID_BASE 2048	/* 0x800 */

typedef enum {
    CA_POLICER_MODE_DISABLE         = 0,    /* policer disabled                     */
    CA_POLICER_MODE_SRTCM           = 1,    /* rfc2697 mode, not support in CS8160  */
    CA_POLICER_MODE_TRTCM           = 2,    /* rfc2698 mode                         */
    CA_POLICER_MODE_TRTCMDS         = 3,    /* rfc4115 (cflag = 0)                  */
    CA_POLICER_MODE_TRTCMDS_COUPLED = 4,    /* rfc4115 with coupled flag (cflag = 1)*/
    CA_POLICER_MODE_RATE_ONLY       = 5     /* rate limiter mode, no color marking  */
} ca_policer_mode_t;

typedef struct {
    ca_policer_mode_t mode; /* policer mode            */
    ca_boolean_t      pps;  /* True - PPS mode, false - PBS mode */
    ca_uint32_t       cir;  /* commited rate           */
    ca_uint32_t       cbs;  /* commited burst size     */
    ca_uint32_t       pir;  /* eir for rfc4115         */
    ca_uint32_t       pbs;  /* ebs for rfc4115&rfc2697 */
} ca_policer_t;

typedef enum {
    CA_POLICER_MC  = 0,     /* multicast                        */
    CA_POLICER_BC  = 1,     /* broadcast                        */
    CA_POLICER_DLF = 2,     /* destination lookup failure (DLF) */
    CA_POLICER_UMC = 3      /* unknown multicast                */
} ca_policer_packet_type_t;

typedef struct {
    ca_uint32_t green_packets;  /* green color packets, unit - packet  */
    ca_uint32_t yellow_packets; /* yellow color packets, unit - packet */
    ca_uint32_t red_packets;    /* red color packets, unit - packet    */
} ca_policer_stat_t;

typedef struct {
    ca_uint32_t overhead;       /* IPG+preamble, for bandwidth adjustment */
} ca_policer_config_t;

typedef struct {
    ca_uint32_t overhead;       /* IPG+preamble, for bandwidth adjustment */
} ca_shaper_config_t;

typedef struct {
    ca_boolean_t enable;        /* enable shaper or not              */
    ca_boolean_t pps;           /* True - PPS mode, false - PBS mode */
    ca_uint32_t  rate;          /* commited rate                     */
    ca_uint32_t  burst_size;    /* commited burst size               */
} ca_shaper_t;

ca_status_t ca_policer_config_set (
    CA_IN ca_device_id_t         device_id,
    CA_IN ca_policer_config_t *config
);

ca_status_t ca_policer_config_get (
    CA_IN  ca_device_id_t         device_id,
    CA_OUT ca_policer_config_t *config
);

ca_status_t ca_port_policer_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_policer_t *policer
);

ca_status_t ca_port_policer_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_policer_t *policer
);

ca_status_t ca_port_policer_stat_get(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_port_id_t      port_id,
    CA_IN  ca_boolean_t      read_keep,
    CA_OUT ca_policer_stat_t *stats
);

ca_status_t ca_port_policer_stat_clear(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id
);

ca_status_t ca_flow_policer_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_uint16_t  flow_id,
    CA_IN ca_policer_t *policer
);

ca_status_t ca_flow_policer_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint16_t  flow_id,
    CA_OUT ca_policer_t *policer
);

ca_status_t ca_group_policer_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_uint16_t  group_id,
    CA_IN ca_policer_t *policer
);

ca_status_t ca_group_policer_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_uint16_t  group_id,
    CA_OUT ca_policer_t *policer,
    CA_OUT ca_uint16_t  *members,
    CA_OUT ca_uint16_t  *member_num
);

ca_status_t ca_group_member_add(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint16_t group_id,
    CA_IN ca_uint16_t flow_id
);

ca_status_t ca_group_member_delete(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint16_t group_id,
    CA_IN ca_uint16_t flow_id
);

ca_status_t ca_group_member_delete_all(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint16_t group_id
);

ca_status_t ca_flow_policer_stat_get(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_uint16_t       flow_id,
    CA_IN  ca_boolean_t      read_keep,
    CA_OUT ca_policer_stat_t *stats
);

ca_status_t ca_flow_policer_stat_clear(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint16_t flow_id
);

ca_status_t ca_group_policer_stat_get(
    CA_IN  ca_device_id_t       device_id,
    CA_IN  ca_uint16_t       group_id,
    CA_OUT ca_policer_stat_t *stats
);


ca_status_t ca_shaper_config_set (
    CA_IN ca_device_id_t        device_id,
    CA_IN ca_shaper_config_t *config
);

ca_status_t ca_shaper_config_get (
    CA_IN  ca_device_id_t        device_id,
    CA_OUT ca_shaper_config_t *config
);

ca_status_t ca_port_shaper_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_shaper_t  *shaper
);

ca_status_t ca_port_shaper_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_OUT ca_shaper_t  *shaper
);

ca_status_t ca_queue_shaper_set (
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  queue_id,
    CA_IN ca_shaper_t  *shaper
);

ca_status_t ca_queue_shaper_get (
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  queue_id,
    CA_OUT ca_shaper_t  *shaper
);

#endif

