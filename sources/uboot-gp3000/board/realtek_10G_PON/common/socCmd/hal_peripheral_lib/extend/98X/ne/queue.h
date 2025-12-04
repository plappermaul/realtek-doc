#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "ca_types.h"


#define  CA_QUEUE_WRED_POINT_COUNT          (16)
#define  CA_QUEUE_WEIGHT_STRICT_PRIORITY    (0)
#define  CA_QUEUE_COUNT                     (8)

typedef enum {
    CA_QUEUE_WRED_COLOR_GREEN  = 0, /* green  */
    CA_QUEUE_WRED_COLOR_YELLOW = 1, /* yellow */
} ca_queue_wred_color_t;


typedef struct {
    ca_uint32_t marked_dp[CA_QUEUE_WRED_POINT_COUNT];   /*drop percentage of each point*/
    ca_uint32_t unmarked_dp[CA_QUEUE_WRED_POINT_COUNT]; /*drop percentage of each point*/
} ca_queue_wred_profile_t;

typedef enum {
    CA_SCHEDULE_STRICT_PRIORITY,
    CA_SCHEDULE_DEFICIT_WEIGHT_ROUND_ROBIN,
    CA_SCHEDULE_DEFICIT_ROUND_ROBIN,
    CA_SCHEDULE_WEIGHT_ROUND_ROBIN,
    CA_SCHEDULE_ROUND_ROBIN
} ca_queue_schedule_mode_t;

typedef struct {
    ca_uint32_t weights[CA_QUEUE_COUNT]; /*schedule weight of each queue*/
} ca_queue_weights_t;


typedef struct {
    ca_uint32_t forwarded;          /* total forwarded packets, unit - packets       */
    ca_uint32_t all_droped;         /* total dropped packets, unit - packets         */
    ca_uint32_t tail_droped;        /* tail-drop dropped packets, unit - packets     */
    ca_uint32_t marked_droped;      /* WRED dropped marked packets, unit - packets   */
    ca_uint32_t unmarked_droped;    /* WRED dropped unmarked packets, unit - packets */
} ca_queue_stat_t;

ca_status_t ca_queue_length_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  queue,
    CA_IN ca_uint32_t  length
);

ca_status_t ca_queue_length_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  queue,
    CA_OUT ca_uint32_t  *length
);

ca_status_t ca_queue_pause_threshold_set(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  queue,
    CA_IN ca_uint32_t  xon,
    CA_IN ca_uint32_t  xoff
);

ca_status_t ca_queue_pause_threshold_get(
    CA_IN  ca_device_id_t  device_id,
    CA_IN  ca_port_id_t port_id,
    CA_IN  ca_uint32_t  queue,
    CA_OUT ca_uint32_t  *xon,
    CA_OUT ca_uint32_t  *xoff
);

ca_status_t ca_queue_schedule_set(
    CA_IN ca_device_id_t              device_id,
    CA_IN ca_port_id_t             port_id,
    CA_IN ca_queue_schedule_mode_t mode,
    CA_IN ca_queue_weights_t       *weights
);

ca_status_t ca_queue_schedule_get(
    CA_IN  ca_device_id_t              device_id,
    CA_IN  ca_port_id_t             port_id,
    CA_OUT ca_queue_schedule_mode_t *mode,
    CA_OUT ca_queue_weights_t       *weights
);

ca_status_t ca_queue_wred_set(
    CA_IN ca_device_id_t             device_id,
    CA_IN ca_port_id_t            port_id,
    CA_IN ca_uint32_t             queue,
    CA_IN ca_queue_wred_profile_t *profile
);

ca_status_t ca_queue_wred_get(
    CA_IN  ca_device_id_t             device_id,
    CA_IN  ca_port_id_t            port_id,
    CA_IN  ca_uint32_t             queue,
    CA_OUT ca_queue_wred_profile_t *profile
);

ca_status_t ca_queue_stat_get(
    CA_IN  ca_device_id_t     device_id,
    CA_IN  ca_port_id_t    port_id,
    CA_IN  ca_uint32_t     queue,
    CA_IN  ca_boolean_t    read_keep,
    CA_OUT ca_queue_stat_t *stats
);

ca_status_t ca_queue_stat_clear(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  queue
);

ca_status_t ca_queue_flush
(
    CA_IN ca_device_id_t  device_id,
    CA_IN ca_port_id_t port_id,
    CA_IN ca_uint32_t  queue 
);


#endif


