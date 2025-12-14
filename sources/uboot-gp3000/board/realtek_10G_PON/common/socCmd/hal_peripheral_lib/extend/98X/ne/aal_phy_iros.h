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
 * aal_l2_vlan_iros.h: L2 VLAN AAL API exported for iROS
 *
 * NOTE: NEVER include this file in this driver. This is for iROS only.
 */
#ifndef __AAL_PHY_H__
#define __AAL_PHY_H__

#include "ca_types.h"

ca_status_t aal_mdio_read (
    CA_IN       ca_device_id_t             device_id,
    CA_IN      ca_uint8_t              st_code,
    CA_IN      ca_uint8_t              phy_port_addr,
    CA_IN      ca_uint8_t              reg_dev_addr,
    CA_IN      ca_uint16_t              addr,
    CA_OUT   ca_uint16_t             *data);

ca_status_t aal_mdio_read_indirect (
    CA_IN       ca_device_id_t             device_id,
    CA_IN       ca_uint8_t             phy_port_addr,
    CA_IN       ca_uint8_t             dev_type,
    CA_IN       ca_uint16_t             addr,
    CA_OUT      ca_uint16_t             *data);

ca_status_t aal_mdio_write_indirect(
   CA_IN       ca_device_id_t             device_id,
   CA_IN       ca_uint8_t             phy_port_addr,
   CA_IN       ca_uint8_t             dev_addr,
   CA_IN       ca_uint16_t             addr,
   CA_IN       ca_uint16_t             data);


#endif /* __AAL_PHY_H__ */

