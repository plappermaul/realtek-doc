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
#ifndef __CA_NI_PROC_H__
#define __CA_NI_PROC_H__

#if 0//yocto
#include <linux/proc_fs.h>
#else//sd1 uboot - TBD
#endif

enum CA_NI_DEBUG {
	NI_DBG_DIASBLE		= 0,
	NI_DBG_NI		= 0x00000001,
	NI_DBG_IRQ		= 0x00000002,
	NI_DBG_LSO		= 0x00000004,
	NI_DBG_DUMP_RX		= 0x00000008,
	NI_DBG_DUMP_TX		= 0x00000010,
	NI_DBG_IOCTL		= 0x00000020,
	NI_DBG_VIR_NI		= 0x00000040,
	NI_DBG_ETHTOOL		= 0x00000080,
	NI_DBG_PHY		= 0x00000100,
	NI_DBG_NETLINK		= 0x00000200,

	NI_DBG_MAX		= NI_DBG_PHY,
};

int ca_ni_proc_init(void);
void ca_ni_proc_exit(void);


#endif /* __CA_NI_PROC_H__ */

