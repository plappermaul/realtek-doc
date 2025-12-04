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
#ifndef __AAL_INIT_H__
#define __AAL_INIT_H__

#include "ca_types.h"

ca_status_t aal_init(ca_device_id_t device_id);
void aal_exit(void);

void dumpTxMIB(ca_uint32_t ca_ni_port);

void dumpRxMIB(ca_uint32_t ca_ni_port);

//+Andy
void fdb_sram_rw_test(ca_uint32 reg_value);

#endif /* __AAL_INIT_H__ */
