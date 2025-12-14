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
#ifndef __AAL_L3FE_H__
#define __AAL_L3FE_H__

#include "ca_types.h"

/* function for testing with iROS */
ca_status_t aal_l3fe_dump_lpb(
		CA_IN ca_device_id_t device_id);

#endif /* __AAL_L3FE_H__ */
