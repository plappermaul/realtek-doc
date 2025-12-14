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
/*
 * aal_qm.h: Hardware Abstraction Layer for Layer-3 Queue Manager to access hardware regsiters
 *
 */
#ifndef __AAL_L3QM_H__
#define __AAL_L3QM_H__

#include "ca_types.h"

typedef union {
  ca_uint32     wrd ;
} QM_QM_VOQ_INFO_MEMORY_DATA0_t;

typedef union {
  ca_uint32     wrd ;
} QM_QM_VOQ_INFO_MEMORY_DATA1_t;

typedef union {
  ca_uint32     wrd ;
} QM_QM_VOQ_INFO_MEMORY_DATA2_t;

typedef union {
  ca_uint32     wrd ;
} QM_QM_HEADER_MEMORY_DATA0_t;

typedef union {
  ca_uint32     wrd ;
} QM_QM_HEADER_MEMORY_DATA1_t;

typedef union {
  ca_uint32     wrd ;
} QM_QM_BID_PADDR_LKUP_DATA_t;

/* function for testing with iROS */
ca_status_t aal_l3qm_get_voq_info_mem(	
		CA_IN ca_device_id_t device_id, 
		CA_IN ca_uint16_t addr, 
		CA_OUT QM_QM_VOQ_INFO_MEMORY_DATA0_t *data0,
		CA_OUT QM_QM_VOQ_INFO_MEMORY_DATA1_t *data1,
		CA_OUT QM_QM_VOQ_INFO_MEMORY_DATA2_t *data);

ca_status_t aal_l3qm_get_header_mem(
		CA_IN ca_device_id_t device_id, 
		CA_IN ca_uint16_t addr, 
		CA_OUT QM_QM_HEADER_MEMORY_DATA0_t *data0,
		CA_OUT QM_QM_HEADER_MEMORY_DATA1_t *data1);

ca_status_t aal_l3qm_get_bid_paddr(CA_IN ca_device_id_t device_id,
		CA_IN ca_uint16_t addr,
		CA_OUT QM_QM_BID_PADDR_LKUP_DATA_t *data);

ca_status_t aal_l3qm_get_error_drop_cntr(
		CA_IN ca_device_id_t device_id);

ca_status_t aal_l3qm_get_tx_rx_cntr(
		CA_IN ca_device_id_t device_id);

ca_status_t aal_l3qm_dump_epps(
		CA_IN ca_device_id_t device_id);

ca_status_t aal_l3qm_dump_empty_buffers(
		CA_IN ca_device_id_t device_id);

#endif /* __AAL_L3QM_H__ */
