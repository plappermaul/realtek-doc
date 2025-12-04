#ifndef __SW_SHAPER_H__
#define __SW_SHAPER_H__

#include "rate.h"
#include "classifier.h"

#define CA_SW_SHAPER_MAX        	256
#define MAX_SHAPER_RATE			((ca_uint32_t) 2*1024*1024*1024)	/* assume max rate is 2Gbps */
#define TOKEN_COUNT_FOR_MAX_RATE	(MAX_SHAPER_RATE/8)	/* totoal token count for 2Gbps */
#define RATE_TO_TOKEN_COUNT(a)		(a/8)			/* convert rate to token count */
#define SEC_TO_MSEC(a)			(a*1000)
#define GET_TO_LIST_END(a)		while (a->p_next) a=a->p_next


/* for software shaper */
typedef struct {
	ca_uint8_t  word3   :1;
	ca_uint8_t  word2   :1;
	ca_uint8_t  word1   :1;
	ca_uint8_t  word0   :1;
} ca_sw_id_mask_t;

typedef struct ca_sw_shaper_entry {
	ca_uint8_t                  sw_shaper_id;    /* caller specified sw shaper id (0-255) */
	ca_port_id_t                internal_port;   /* 0x20-0x3f */
	ca_uint8_t                  prio;            /* CoS 0-7 */
	ca_shaper_t                 shaper;
	ca_sw_id_mask_t             sw_id_mask;      /* reserved */
	ca_uint16_t                 sw_id[4];        /* reserved. Can be used in combination with sw_id_mask for flow identification*/
	ca_classifier_action_t      *action;         /* not use, reserved for future actions. Set to NULL */
} ca_sw_shaper_entry_t;

typedef struct ca_sw_shaper_ipc_entry {
	ca_uint32_t                 sw_shaper_id;    /* caller specified sw shaper id (0-255), use ca_uint32_t for 4 byte alignment */
	ca_uint32_t                 token_per_msec;
	ca_uint32_t		    default_shaper_token_count_per_msec;
	ca_uint32_t		    burst_token_per_msec;
} ca_sw_shaper_ipc_entry_t;

typedef struct ca_sw_shaper_ipc_init {
        ca_uint32_t             max_shaper_supported;
        ca_uint32_t             default_shaper_token_count_per_msec;
} ca_sw_shaper_ipc_init_t;

ca_status_t ca_sw_shaper_entry_add (
	CA_IN  ca_device_id_t          device_id,
	CA_OUT ca_sw_shaper_entry_t *p_sw_shaper
);

ca_status_t ca_sw_shaper_entry_delete (
	CA_IN  ca_device_id_t  device_id,
	CA_OUT ca_uint16_t  sw_shaper_id
);

ca_status_t ca_sw_shaper_delete_all (
	CA_IN  ca_device_id_t device_id
);

ca_status_t ca_sw_shaper_entry_get (
	CA_IN  ca_device_id_t  device_id,
	CA_IN  ca_uint16_t  sw_shaper_id,
	CA_OUT ca_sw_shaper_entry_t *p_sw_shaper
);

ca_status_t ca_sw_shaper_entry_iterate (
	CA_IN  ca_device_id_t   device_id,
	CA_OUT ca_iterator_t *p_return_entry
);

#endif

