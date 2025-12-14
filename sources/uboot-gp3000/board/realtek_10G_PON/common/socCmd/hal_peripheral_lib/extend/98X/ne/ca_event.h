#ifndef _EVENT_IMP_H_
#define _EVENT_IMP_H_

#include "ca_types.h"

typedef ca_uint32_t  ca_mpcp_id_t;

/* definition for event id/message */
typedef enum
{
    CA_EVENT_SC_LINK,
    CA_EVENT_WD_EXPIRED,
    CA_EVENT_HWTIMER_EXPIRED,
    CA_EVENT_GPIO,
    CA_EVENT_ETH_PORT_LINK,             /* Port(PON/UNI) link change event */
    CA_EVENT_EPON_PORT_LINK,
    CA_EVENT_GPON_PORT_LINK,
    CA_EVENT_ETH_EXT_PHY_INTR,
    CA_EVENT_L2_FDB_STATION_MOVE,
    CA_EVENT_L2_FDB_AGING_OUT,
    CA_EVENT_PON_MAC_SEC_STATUS_INTR,
    CA_EVENT_L2_FDB_STATUS_INTR,
    CA_EVENT_L2_FDB_HASH_COLLISION,
    CA_EVENT_L2_FDB_LEARN_LIMIT,
    CA_EVENT_FRAME_DDR_TEST,
    CA_EVENT_EPON_REG_STAT_CHNG,  /* MPCP REG status change event */
    CA_EVENT_OAM_LINK,
    CA_EVENT_OMCC_LINK,

    CA_EVENT_MAX
} ca_event_type_e;
/******************************************************************************
 *                       Event Module Constants
 *****************************************************************************/
#define SDL_EVENT_HOLDOVER_TIME_UNIT    10     /* milli seconds */
#define SDL_EVENT_THROTTLE_TIME_UNIT    10     /* milli seconds */

/******************************************************************************
 *              Typedefs and Datatypes for event data
 *****************************************************************************/

typedef ca_uint32_t (*ca_event_cb_ptr)
(
    ca_device_id_t         device_id,
    ca_uint32_t         event_id,
    ca_void             *event,
    ca_uint32_t         len,
    ca_void             *cookie
);

/* CA_EVENT_SC_LINK */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_port_id_t    port_id;            /* indicates the port id for SC link */
    ca_uint32_t     status;            /* indicates the status of SC link.
                                       {    0x0 : SC link down
                                         0x1 : SC link up
                                         0x2 : download request } */
}ca_event_sc_link_t;

/* CA_EVENT_HARDWARE_TIMER_EXPIRED */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_uint32_t timer_id;            /* indicates which timer expired */
}ca_event_hardware_timer_expired_t;

/* CA_EVENT_GPIO */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_uint32_t gpio_id;            /* indicates the GPIO PIN */
}ca_event_gpio_t;

/* CA_EVENT_ETH_PORT_LINK */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_port_id_t port_id;
    ca_int32_t status; /* see definition for ca_eth_port_link_status_t */
    ca_int32_t duplex;       /* see definition for ca_eth_port_duplex_t*/
    ca_int32_t speed;           /* see definition for ca_eth_port_speed_t */
}ca_event_eth_port_link_t;

/* CA_EVENT_EPON_PORT_LINK */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_port_id_t            port_id;
    ca_boolean_t            status;     /* TRUE = link up, FALSE = link down */
    ca_int32_t              speed;
    ca_boolean_t            dying_gasp; /* TRUE = dying gasp occured */
}ca_event_epon_port_link_t;

typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_port_id_t            port_id;
    ca_boolean_t            status;     /* TRUE = link up, FALSE = link down */
    ca_int32_t              speed;
    ca_uint32_t             reason; /* link down reason */
}ca_event_gpon_port_link_t;


/* CA_EVENT_ETH_EXT_PHY_INTR */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_uint32_t intr_status;
}ca_event_ext_phy_intr_t;

/* CA_EVENT_FDB_STAUS_INTR_T */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_uint32_t     l2_fdb_overflow : 1;
    ca_uint32_t     l2_fdb_hash_collision : 1;
}ca_event_l2_fdb_status_intr_t;

/* CA_EVENT_PON_MAC_SEC_STATUS_INTR_T */
typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_uint32_t    pn_mem_rd_Cerr : 1;        //encryption, 1 bit ECC data read error corrected
    ca_uint32_t    pn_mem_rd_err : 1;        //encryption, multiple ECC error in data read
    ca_uint32_t    cntl_mem_rd_Cerr : 1;         //decryption, 1 bit ECC data read error corrected
    ca_uint32_t    cntl_mem_rd_err : 1;          //decryption, multiple ECC error in data read
    ca_uint32_t    key_hash_mem_rd_Cerr : 1; //TX hash, 1 bit ECC data read error corrected
    ca_uint32_t    key_hash_mem_rd_err : 1;     //RX hash, multiple ECC error in data read
    ca_uint32_t    pn_thld_reached : 1;         //PN threshold value exceeded
}ca_event_mac_intr_t;

typedef enum {/*CODEGEN_IGNORE_TAG*/
    CA_OMCC_LINK_DOWN         = 0,/* OMCC is down */
    CA_OMCC_LINK_UP           = 1 /* OMCC is up */
} ca_omcc_link_status_t;

typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_port_id_t port_id;
    ca_omcc_link_status_t status; /* see definition for ca_omcc_link_status_t */
}ca_event_omcc_link_t;

typedef struct {/*CODEGEN_IGNORE_TAG*/
    ca_uint8    llid_idx;
	ca_uint16_t	llid_value;
	ca_boolean_t	status;			/* 1 - Registered, 0 - Deregistred */
} ca_event_reg_stat_t; /* defined for CA_EVENT_EPON_REG_STAT_CHNG */

/******** Device level events data *************/

/* CA_EVENT_TMR_EXPIRED */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	timer_id;
} sdl_tmr_event_data_s;

/* CA_EVENT_GPIO_INTR */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint32_t	gpio_pin_mask;
} sdl_gpio_event_data_s;

/* CA_EVENT_DMA_ERR */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint8_t	dma_id;
} sdl_dma_event_data_s;

/* CA_EVENT_THROTTLED */
typedef enum {/*CODEGEN_IGNORE_TAG*/
	SDL_EVENT_THRTL_GLOBAL = 0,
	SDL_EVENT_THRTL_PER,
} sdl_event_thrtl_type_e;

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_event_type_e		thrtld_event;
	sdl_event_thrtl_type_e	thrtl_type;
	ca_uint32_t		thrtl_cnt;
} sdl_thrtl_event_data_s;

/************ ETH Port events data **************/
/* CA_EVENT_ETH_EXT_PHY_INTR */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	intr_status;	/* intr_status will be valid only when
					   phy is managed by cortina driver
					   will be set to 0 otherwise
					 */
} sdl_eth_phy_event_info_s;

/* CA_EVENT_ETH_PHY_MODE_CHNG */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_speed_t	speed;
	ca_duplex_t	duplex;
} sdl_eth_phy_mode_chng_data_s;

/* CA_EVENT_ETH_LINK_STATUS_CHNG */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_speed_t	speed;
	ca_duplex_t	duplex;
	ca_boolean_t	link_up;
} sdl_eth_link_status_chng_event_data_s;


/************ PTP events data **************/
/* CA_EVENT_PTP_EGR_TIME_LATCHED
 * CA_EVENT_PTP_INGR_TIME_LATCHED
 * CA_EVENT_PTP_GET_TS
 * CA_EVENT_PTP_SET_TS
 */

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint32_t	ptp_nano_second;	/* ptp_nano_second[31:0]*/

	ca_uint32_t	local_ref_time;		/* Local PON time at which PTP
						 * time was captured
						 */

	ca_uint16_t	local_ref_lsb;		/* Used only in GPON mode */

	ca_uint32_t	ptp_freq_offset;	/* PTP offset */

} sdl_ptp_get_ts_event_data_s;

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	port_num;
	ca_uint32_t	timestamp;
} sdl_ptp_tmr_latch_data_s;




/************ GPON Port events data *************/
/* CA_EVENT_GPON_ONU_STATE_CHNG */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	/* TODO: find a correct place for this definition */
	ca_uint32_t	onu_state;
} sdl_onu_state_event_data_s;

/* CA_EVENT_GPON_ONU_ID */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	onu_id;
} sdl_gpon_onu_id_event_data_s;

/* CA_EVENT_GPON_ALM_xxx */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t	status;
} sdl_gpon_alm_event_data_s;

/* CA_EVENT_GPON_OMCC_STATE */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t	status;
	ca_uint16_t	gem_port_id;
} sdl_gpon_omcc_state_data_s;

/* CA_EVENT_GPON_OMCC_GEM_PORT_ID */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	gem_port_id;
} sdl_gpon_omcc_gem_port_data_s;




typedef enum {/*CODEGEN_IGNORE_TAG*/
	SDL_EPON_HOLDOVER_START = 0x0,
	SDL_EPON_HOLDOVER_END,
} sdl_epon_holdover_event_status_e;

/* CA_EVENT_EPON_SIG_LOSS */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t	link_status;
	ca_boolean_t	pca_an_done;
	ca_duplex_t	lp_duplex;
	ca_boolean_t	pca_in_sync;
} sdl_epon_los_event_data_s;

/* CA_EVENT_EPON_PCA_RX_SYNC_STAT_CHNG */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t	pca_in_sync;
} sdl_epon_pca_sync_status_s;

/* CA_EVENT_EPON_REG_STAT_CHNG */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	llid;
	ca_uint16_t	assigned_llid;
	ca_boolean_t	status;			/* 1 - Registered, 0 - Deregistred */
} sdl_epon_llid_event_data_s;

/* CA_EVENT_EPON_HOLDOVER */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	sdl_epon_holdover_event_status_e	status;
} sdl_epon_holdover_event_data_s;



/* CA_EVENT_FE_STN_MOVE */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint8_t	stn_move_indx;
} sdl_fe_stn_move_event_data_s;

/************ EEE event data *************/
typedef struct {/*CODEGEN_IGNORE_TAG*/
	/* TODO: find a right place for these def */
	ca_uint32_t	eee_dir;
	ca_uint32_t	eee_state;
} sdl_eee_event_data_s;

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_uint16_t	low_thresh;		/* Queue bitmap */
	ca_uint16_t	high_thresh;
} sdl_eee_queue_status_event_data_s;

/*UNI auto neg status event */
typedef struct{/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t	status;
} sdl_eth_phy_auto_nego_status_s;


typedef struct{/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t	sync_status;
	ca_uint16_t	mode;
} sdl_pon_mode_event_data_s;

typedef union {/*CODEGEN_IGNORE_TAG*/
	ca_uint32_t				data;
	sdl_tmr_event_data_s			tmr_data;

	sdl_gpio_event_data_s			gpio_data;
	sdl_dma_event_data_s			dma_data;
	sdl_thrtl_event_data_s			thrtl_data;
	sdl_eth_phy_event_info_s		phy_intr_data;
	sdl_eth_phy_mode_chng_data_s		phy_mode_data;
	sdl_eth_link_status_chng_event_data_s	link_chng_data;
	sdl_eth_phy_auto_nego_status_s		auto_nego_status;
	sdl_pon_mode_event_data_s		pon_mode_data;


	sdl_ptp_get_ts_event_data_s		get_ts_data;
	sdl_ptp_tmr_latch_data_s		ptp_latch_data;



	sdl_epon_los_event_data_s		los_data;
	sdl_epon_pca_sync_status_s		pca_sync_status;
	sdl_epon_llid_event_data_s		llid_data;
	sdl_epon_holdover_event_data_s		hold_data;


	sdl_onu_state_event_data_s		onu_state_data;
	sdl_gpon_alm_event_data_s		gpon_alm_data;
	sdl_gpon_onu_id_event_data_s		onu_id_data;
	sdl_gpon_omcc_state_data_s		omcc_state;
	sdl_gpon_omcc_gem_port_data_s		omcc_gem_data;


	sdl_fe_stn_move_event_data_s		stn_move_data;
	sdl_eee_event_data_s			eee_data;
	sdl_eee_queue_status_event_data_s	eee_queue_status;
} sdl_event_data_u;

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_device_id_t		device_id;
	aal_port_t		port_num;
	ca_event_type_e		event_type;
	ca_uint32_t		user_data;
	sdl_event_data_u	event_data;
} sdl_event_info_s;

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t		user_app;	/* user app/kernel app flag */
	ca_uint8_t		context_id;
	ca_uint8_t		priority;
	struct {
		ca_uint8_t	context_id	:1;
		ca_uint8_t	evt_thread_id	:1;
	} valid;
	ca_uint32_t		app_id;
	ca_uint32_t		chan_hndl;
	ca_uint32_t		src;		/* uid */
	ca_uint32_t		evt_thread_id;
	ca_uint32_t		handle;		/* pointer to the context handle */
} ca_event_app_context_info_t;


/******************************************************************************
 *              Typedefs and Datatypes used in events
 *****************************************************************************/
typedef void (/*CODEGEN_IGNORE_TAG*/*sdl_event_handler_t) (const sdl_event_info_s *p_info);


/* Event notification context */
typedef enum {/*CODEGEN_IGNORE_TAG*/
	SDL_EVENT_CONTEXT_USER = 0x0,
	SDL_EVENT_CONTEXT_URGENT_INTR,
	SDL_EVENT_CONTEXT_HIGH_PRI_INTR,
	SDL_EVENT_CONTEXT_NORMAL_INTR,
	SDL_EVENT_CONTEXT_SLEEPABLE,
	SDL_EVENT_CONTEXT_MAX,
} sdl_event_context_e;

/* Event registration configuration */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	sdl_event_handler_t	callback;
	ca_uint32_t		user_data;
	sdl_event_context_e	context;
	ca_uint8_t		context_id;
	ca_uint8_t		priority;	/* range 0 to 7 (0-lowest)*/
	ca_uint32_t		port_type_bitmap;
	ca_boolean_t		all_ports;
	ca_uint16_t		port_num;

	ca_uint16_t		gem_port_id;

	ca_uint16_t		llid;

} ca_event_cfg_s;

/* Event status for polling */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t		status;		/* status of event set or clear */
	sdl_event_data_u	event_data;
} ca_event_status_s;

/* Event holdover configurations */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t		enable;
	ca_uint16_t		port_num;
	ca_uint16_t		sub_port;
	ca_uint16_t		holdover_time;
} ca_event_holdover_cfg_s;

/* Event throttle configurations */
typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t		enable;
	ca_uint32_t		count;
	ca_uint32_t		interval;
} ca_event_throttle_cfg_s;

typedef struct {/*CODEGEN_IGNORE_TAG*/
	ca_boolean_t		enable;
	ca_boolean_t		per_port;
	ca_uint32_t		count;
	ca_uint32_t		interval;
} ca_intr_throttle_cfg_s;


typedef struct
{
    ca_port_id_t port_id;
    ca_uint32_t status;
} ca_event_port_link_t;

typedef struct
{
    ca_port_id_t port_id;
    ca_mac_addr_t mac;
    ca_uint16_t rsvd;
    ca_mpcp_id_t mpcp_id;
    ca_llid_t llid;
    ca_uint32_t status;
}ca_event_oam_link_t;

typedef struct
{
    ca_port_id_t port_id;
    ca_mac_addr_t mac;
    ca_uint16_t rsvd;
    ca_mpcp_id_t mpcp_id;
    ca_llid_t llid;
    ca_uint32_t flag;     /*0x0: MPCP registration.  0x1: MPCP re-registration.  0x2: MPCP de-registration */
    ca_uint32_t reason;
} ca_event_epon_mpcp_reg_change_t;

typedef struct{
    ca_uint32_t    status;
    ca_uint32_t    addr;
    ca_uint32_t    exp_v;
    ca_uint32_t    real_v;
} ca_evt_frame_ddr_t;  /*CA_EVENT_FRAME_DDR_TEST*/

typedef struct {
    ca_uint32_t timer_id;
} ca_event_hw_timer_t;

typedef struct
{
    ca_uint32_t pps_timer;
} ca_event_ptp_gps_t;

/* definition for event infrastructure */

#ifndef CA_EVT_OK
#define CA_EVT_OK           (0)
#define CA_EVT_ERROR        (-1)

typedef struct _ca_event_cfg_t
{
    ca_uint32_t throttling_ena;
    ca_uint32_t threshold;
    ca_uint32_t interval;
} ca_event_cfg_t;

typedef enum
{
    EVT_CFG_COUNTER_GET,
    EVT_CFG_COUNTER_CLR,
    EVT_CFG_COUNTER_CLR_ALL,
    EVT_CFG_SET,
    EVT_CFG_GET,
    EVT_CFG_DUMP,
    MAX_EVT_CFG_TYPE
} EVT_CFG_TYPE_ENUM;

typedef struct _ca_event_cfg_local_t
{
    ca_int32_t cfg_type;
    ca_int32_t evt_type;
    ca_uint32_t dest;
    ca_event_cfg_t evt_cfg;
    ca_uint32_t counter;
    ca_boolean read_clear;
} ca_event_cfg_local_t;

typedef void (/*CODEGEN_IGNORE_TAG*/ *event_handler)(ca_device_id_t device_id, ca_int32_t event_type, void *msg, ca_int32_t len);
typedef void (/*CODEGEN_IGNORE_TAG*/ *event_to_remote)(ca_device_id_t device_id, ca_int32_t event_type, void *msg, ca_int32_t len);
typedef void (/*CODEGEN_IGNORE_TAG*/ *event_cfg_func)(ca_device_id_t device_id, ca_event_cfg_local_t *cfg);
#endif

#define MAX_DEST_NUM    8
extern void cap_plat_event_init(/*CODEGEN_IGNORE_TAG*/void);

extern ca_status_t ca_event_register (/*CODEGEN_IGNORE_TAG*/
CA_IN		ca_device_id_t				device_id,
CA_IN		ca_uint32_t				event_id,
CA_IN		ca_event_cb_ptr			event_cb,
CA_IN		ca_void					*cookie);

extern ca_status_t ca_event_deregister (/*CODEGEN_IGNORE_TAG*/
CA_IN		ca_device_id_t				device_id,
CA_IN		ca_uint32_t				event_id,
CA_IN		ca_event_cb_ptr			event_cb);

ca_status_t ca_event_counter_get (/*CODEGEN_IGNORE_TAG*/
CA_IN		ca_device_id_t				device_id,
CA_IN		ca_uint32_t				event_id,
CA_OUT		ca_uint32_t				*counter,
CA_IN		ca_boolean_t		    read_clear);

ca_status_t ca_event_counter_clear (/*CODEGEN_IGNORE_TAG*/
CA_IN        ca_device_id_t                device_id,
CA_IN        ca_uint32_t                event_id);


ca_status_t ca_event_counter_clear_all (/*CODEGEN_IGNORE_TAG*/
CA_IN        ca_device_id_t                device_id);

ca_int32_t ca_event_set
    (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t evt_type,
    CA_IN ca_event_cfg_t *evt_cfg
    );
ca_int32_t ca_event_get
    (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32_t evt_type,
    CA_OUT ca_event_cfg_t *evt_cfg
    );
ca_int32 ca_event_init
    (/*CODEGEN_IGNORE_TAG*/
    ca_int32 max_event_type,
    event_to_remote send_evt_to_remote,
    event_cfg_func send_cfg_to_local
    );
ca_int32 ca_event_suprs_attach
    (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_int32 evt_type,
    CA_IN void (*enable)(void *data),
    CA_IN void (*disable)(void *data),
    CA_IN void *data
    );
ca_int32 ca_event_send
    (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t device_id,
    CA_IN ca_int32 evt_type,
    CA_IN void *msg,
    CA_IN ca_int32 msg_len
    );


ca_int32 ca_event_remote_register
    (/*CODEGEN_IGNORE_TAG*/
    CA_IN ca_device_id_t device_id,
    CA_IN ca_uint32 dest,
    CA_IN ca_int32 evt_type,
    CA_IN event_handler handler
    );
void ca_event_dump(/*CODEGEN_IGNORE_TAG*/void);
#endif

