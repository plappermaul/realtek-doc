#ifndef __CA_US_RATE_MANAGER_H__
#define __CA_US_RATE_MANAGER_H__

#include "rate.h"

#define CA_US_RATE_MANAGER_MAX          		128
#define CA_MAX_PE_FWD_RATE				4000000
#define CA_US_RATE_MANAGER_MAX_DEST_VOQ  		383
#define CA_US_RATE_MANAGER_MIN_DEST_VOQ  		256
#define CA_US_RATE_MANAGER_DEFAULT_PRIORITY             0
#define CA_US_RATE_MANAGER_DEFAULT_DEST_VOQ             CA_US_RATE_MANAGER_MAX_DEST_VOQ


typedef struct ca_us_rate_manager_entry {
	ca_uint8_t			rate_mgr_id;	 	/* us_rate_manager id (0-255) returned by us_rate_manager add */
	ca_uint32_t 			priority;
	ca_uint32_t			classifier_index;
	ca_uint32_t			flow_id;		/* policer id */
	ca_uint8_t			f_default_manager;
	ca_uint32_t			rate;			/* unit is Kbps */				
} ca_us_rate_manager_entry_t;

typedef struct ca_us_rate_manager_ipc_entry {
	ca_uint32_t			dest_VoQ;
	ca_uint32_t                     rate;                   /* unit is Kbps */
} ca_us_rate_manager_ipc_entry_t;

typedef struct ca_us_rate_manager_ipc_init {
	ca_uint32_t			dest_VoQ;
	ca_uint32_t                     default_rate;           /* unit is Kbps */
	ca_uint32_t                     max_rate;               /* unit is Kbps */
} ca_us_rate_manager_ipc_init_t;

ca_status_t ca_us_rate_manager_entry_add (
    CA_IN  ca_device_id_t               device_id,
    CA_IN  ca_us_rate_manager_entry_t   *p_rate_mgr
);

ca_status_t ca_us_rate_manager_entry_delete (
	CA_IN  ca_device_id_t  device_id,
	CA_IN ca_uint32_t  sw_rate_limiter_id
);

ca_status_t ca_us_rate_manager_delete_all (
	CA_IN  ca_device_id_t device_id
);

ca_status_t ca_us_rate_manager_entry_get (
	CA_IN  ca_device_id_t  device_id,
	CA_IN  ca_uint32_t  sw_rate_limiter_id,
	CA_OUT ca_us_rate_manager_entry_t *p_rate_limiter
);

ca_status_t ca_us_rate_manager_entry_iterate (
	CA_IN  ca_device_id_t   device_id,
	CA_OUT ca_iterator_t *p_return_entry
);

#endif

