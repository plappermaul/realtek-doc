#ifndef __PTP_H__
#define __PTP_H__

#include "ca_types.h"

typedef struct ca_ptp_local_clock {
	ca_uint64_t	sec;	/* 48 bits sec field of PTP local clock */
	ca_uint32_t	nsec;	/* 32 bits nsec field of PTP local clock */
} ca_ptp_local_clock_t;

typedef enum ca_ptp_pkt_type {
	PTP_SYNC	= 0,	/* message timestamped by HW */
	PTP_DELAY_REQ	= 1,	/* message timestamped by HW */
	PTP_FOLLOWUP	= 2,	/* message NOT timestamped by HW, time stamp contained in payload */
	PTP_DELAY_RESP	= 3,	/* message NOT timestamped by HW, time stamp contained in payload */
} ca_ptp_pkt_type_t;

typedef enum{
    PTP_TX_PORT0 = 0,
    PTP_TX_PORT1 = 1,
    PTP_TX_PORT2 = 2,
    PTP_TX_PORT3 = 3,
    PTP_TX_PORT4 = 4,
    PTP_TX_PORT5 = 5,
    PTP_TX_PORT6 = 6,
    PTP_TX_PORT7 = 7,
}ca_ptp_tx_port_t;

ca_status_t ca_ptp_port_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_port_id_t		port_id,
	CA_IN	ca_boolean_t		enable);

ca_status_t ca_ptp_port_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_port_id_t		port_id,
	CA_OUT	ca_boolean_t		*enable);

ca_status_t ca_ptp_local_clock_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ptp_local_clock_t	local_clock);

ca_status_t ca_ptp_local_clock_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ptp_local_clock_t	*local_clock);

ca_status_t ca_ptp_local_clock_delta_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ptp_local_clock_t	local_clock_delta);

ca_status_t ca_ptp_local_clock_delta_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_ptp_local_clock_t	*local_clock_delta);

ca_status_t ca_ptp_local_clock_freq_set (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_int32_t		freq);

ca_status_t ca_ptp_local_clock_freq_get (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_int32_t		*freq);

ca_status_t ca_ptp_port_pkt_send (
	CA_IN	ca_device_id_t		device_id,
	CA_IN	ca_port_id_t		port,
	CA_IN	ca_ptp_pkt_type_t	pkt_type,
	CA_IN	ca_uint8_t		pkt_len,
	CA_IN	ca_uint8_t		*pkt_buffer);

#endif /* __PTP_H__ */

