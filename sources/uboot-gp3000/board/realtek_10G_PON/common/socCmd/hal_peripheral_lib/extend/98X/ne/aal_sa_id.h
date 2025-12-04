/*
 * Copyright (c) Cortina-Access Limited 2016.  All rights reserved.
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
 * aal_sa_id.h: Security Association (SA ID) management for Packet Engine
 * This is not related to any HW registers; pure software.
 *
 */
#ifndef __AAL_SA_ID_H__
#define __AAL_SA_ID_H__

#include "tunnel.h"
#include "aal_table.h"
#include "aal_common.h"

#define SA_ID_ENTRY_MAX			256

/* SA ID 0~23 are for encrypted tunnel which uses Crypto Engine.
 * SA ID 24~max are for plaintext tunnel.
 */
#define SA_ID_ENCRYPT_MIN		0
#define SA_ID_ENCRYPT_MAX		23
#define SA_ID_PLAINTEXT_MIN		(SA_ID_ENCRYPT_MAX + 1)
#define SA_ID_PLAINTEXT_MAX		(SA_ID_ENTRY_MAX - 1)

#define SA_ID_INVALID			0xffff

/***************************************************************************/

typedef struct aal_sa_id_entry_s {
	ca_boolean_t			valid;
	ca_tunnel_direction_t		direction;
	ca_boolean_t			is_encrypted;	/* tunnel is encrypted or not */
	ca_tunnel_type_t		tunnel_type;
} __attribute__((packed)) aal_sa_id_entry_t;

/***************************************************************************/

int __aal_sa_id_tbl_init(void);
void __aal_sa_id_tbl_exit(void);

#endif /* __AAL_SA_ID_H__ */

