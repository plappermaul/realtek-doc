#ifndef __SW_RATE_LIMITER_H__
#define __SW_RATE_LIMITER_H__

#include "rate.h"
#include "classifier.h"

#define CA_SW_RATE_LIMITER_MAX        	256
#define MAX_PE_FWD_RATE			4000000
#define CA_SW_RATE_LIMITER_DEFAULT_VOQ	0

typedef struct ca_sw_rate_limiter_entry {
	ca_uint8_t			sw_rate_limiter_id;    	/* sw rate limiter id (0-255) returned by rate limiter add */
	ca_uint32_t			dest_VoQ;
	ca_uint32_t			classifier_index;
	ca_uint32_t			flow_id;		/* policer id */
	ca_uint8_t			f_default_limiter;
	ca_uint32_t			rate;			/* unit is Kbps */				
} ca_sw_rate_limiter_entry_t;

typedef struct ca_sw_rate_limiter_ipc_entry {
	ca_uint32_t			dest_VoQ;
} ca_sw_rate_limiter_ipc_entry_t;

typedef struct ca_sw_rate_limiter_ipc_init {
	ca_uint32_t			dest_VoQ;
} ca_sw_rate_limiter_ipc_init_t;

ca_status_t ca_sw_rate_limiter_entry_add (
	CA_IN  ca_device_id_t          	device_id,
	CA_IN  ca_uint32_t		configured_rate,
	CA_IN  ca_uint8_t		default_f
);

ca_status_t ca_sw_rate_limiter_entry_delete (
	CA_IN  ca_device_id_t  device_id,
	CA_IN ca_uint32_t  sw_rate_limiter_id
);

ca_status_t ca_sw_rate_limiter_delete_all (
	CA_IN  ca_device_id_t device_id
);

ca_status_t ca_sw_rate_limiter_entry_get (
	CA_IN  ca_device_id_t  device_id,
	CA_IN  ca_uint32_t  sw_rate_limiter_id,
	CA_OUT ca_sw_rate_limiter_entry_t *p_rate_limiter
);

ca_status_t ca_sw_rate_limiter_entry_iterate (
	CA_IN  ca_device_id_t   device_id,
	CA_OUT ca_iterator_t *p_return_entry
);

#endif

