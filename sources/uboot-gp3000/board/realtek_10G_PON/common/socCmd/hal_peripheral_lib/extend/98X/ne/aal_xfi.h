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

#ifndef __AAL_XFI_H__
#define __AAL_XFI_H__

#include "ca_types.h"
#include "aal_phy.h"
typedef struct {
    ca_uint8_t ber_notify ;
    ca_uint8_t rxidle     ;
    ca_uint8_t clk_rdy    ;
}aal_xfi_status_t;

typedef struct {
    ca_uint8_t txdly           ;
    ca_uint8_t mode_125m       ;
    ca_uint8_t spdsel          ;
    ca_uint8_t dfe_en          ;
    ca_uint8_t leq_en          ;
    ca_uint8_t pdown           ;
    ca_uint8_t rx_en           ;
}aal_xfi_cfg_t;

typedef union {
  struct {
    ca_uint32_t txdly     : 1;
    ca_uint32_t mode_125m : 1;
    ca_uint32_t spdsel    : 1;
    ca_uint32_t dfe_en    : 1;
    ca_uint32_t leq_en    : 1;
    ca_uint32_t pdown     : 1;
    ca_uint32_t rx_en     : 1;
    ca_uint32_t rsvd      : 25;
  } s ;
  ca_uint32_t     data32 ;

}aal_xfi_cfg_mask_t;

extern ca_status_t aal_xfi_cfg_get(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_uint32_t        xfi_idx,
    CA_OUT aal_xfi_cfg_t     *config);

extern ca_status_t aal_xfi_link_status_get(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_uint32_t        xfi_idx,
    CA_OUT ca_boolean_t      *link_up);

extern ca_status_t aal_xfi_cfg_set(
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_uint32_t        xfi_idx,
    CA_IN  aal_xfi_cfg_mask_t mask,
    CA_IN aal_xfi_cfg_t     *config);

extern ca_status_t aal_xfi_status_get(/*AUTO-CLI gen ignore */
    CA_IN  ca_device_id_t        device_id,
    CA_IN  ca_uint32_t        xfi_idx,
    CA_OUT aal_xfi_status_t  *status);

extern ca_status_t aal_xfi_hw_init(/*AUTO-CLI gen ignore */
    CA_IN  ca_device_id_t  device_id);

extern ca_status_t aal_xfi_init(/*AUTO-CLI gen ignore */
    CA_IN  ca_device_id_t  device_id);

extern ca_status_t aal_xfi_speed_set(
    CA_IN  ca_device_id_t device_id, 
    CA_IN  ca_uint32_t pcs_index, 
    CA_IN  ca_uint32_t speed);

extern ca_status_t aal_xfi_speed_get(
    CA_IN  ca_device_id_t device_id, 
    CA_IN  ca_uint32_t pcs_index, 
    CA_OUT ca_uint32_t *speed);

extern ca_status_t aal_xfi_do_hw_autoneg(/*AUTO-CLI gen ignore */
    CA_IN  ca_device_id_t device_id, 
    CA_IN  aal_phy_sxgmii_pcs_control_index_t pcs_index);


#endif /* __AAL_XFI_H__ */

