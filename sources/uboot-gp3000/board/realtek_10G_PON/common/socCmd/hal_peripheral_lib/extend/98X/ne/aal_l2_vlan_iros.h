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
#ifndef __AAL_L2_VLAN_IROS_H__
#define __AAL_L2_VLAN_IROS_H__

#include "ca_types.h"

typedef enum{
    AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_A  = 0,
    AAL_L2_VLAN_TX_TPID_SEL_CSR_TPID_B  = 1,
    AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_A   = 2,
    AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_B   = 3,
    AAL_L2_VLAN_TX_TPID_SEL_MAX         = AAL_L2_VLAN_TX_TPID_SEL_OLD_CSR_B,
}aal_l2_vlan_tx_tpid_sel_t;

typedef struct{
    aal_l2_vlan_tx_tpid_sel_t svlan;
    aal_l2_vlan_tx_tpid_sel_t cvlan;
    aal_l2_vlan_tx_tpid_sel_t inner_svlan;
    aal_l2_vlan_tx_tpid_sel_t inner_cvlan;
}aal_l2_vlan_port_tx_tpid_sel_t;

ca_status_t aal_l2_vlan_tx_tpid_sel_set(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_IN ca_uint32_t mask,
                                        CA_IN aal_l2_vlan_port_tx_tpid_sel_t  *cfg);

ca_status_t aal_l2_vlan_tx_tpid_sel_get(CA_IN ca_device_id_t device_id,
                                        CA_IN ca_uint32_t port_id,
                                        CA_OUT aal_l2_vlan_port_tx_tpid_sel_t  *cfg);

#endif /* __AAL_L2_VLAN_IROS_H__ */

