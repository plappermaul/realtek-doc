/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_FWDL_H_
#define _MAC_AX_FWDL_H_

#include "../type.h"

struct fwhdr_hdr_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
};

struct fwhdr_section_t {
	u32 dword0;
	u32 dword1;
	u32 dword2;
	u32 dword3;
};

#define FWHDR_HDR_LEN		(sizeof(struct fwhdr_hdr_t))
#define FWHDR_SECTION_LEN	(sizeof(struct fwhdr_section_t))

#define ROMDL_SEG_LEN               0x40000

/* === FW header === */
/* dword0 */
#define FWHDR_CUTID_SH              0
#define FWHDR_CUTID_MSK             0xff
#define FWHDR_CHIPID_SH             8
#define FWHDR_CHIPID_MSK            0xffffff

/* dword1 */
#define FWHDR_VERSION_SH            0
#define FWHDR_VERSION_MSK           0xfff
#define FWHDR_VERSIONTOP_SH         12
#define FWHDR_VERSIONTOP_MSK        0xf
#define FWHDR_SUBVERSION_SH         16
#define FWHDR_SUBVERSION_MSK        0xff
#define FWHDR_SUBINDEX_SH           24
#define FWHDR_SUBINDEX_MSK          0xff

/* dword2 */
#define FWHDR_COMMITID_SH           0
#define FWHDR_COMMITID_MSK          0xffffffff

/* dword3 */
#define FWHDR_SEC_HDR_OFFSET_SH     0
#define FWHDR_SEC_HDR_OFFSET_MSK    0xff
#define FWHDR_SEC_HDR_SZ_SH         8
#define FWHDR_SEC_HDR_SZ_MSK        0xff
#define FWHDR_FWHDR_SZ_SH           16
#define FWHDR_FWHDR_SZ_MSK          0xff
#define FWHDR_FWHDR_VER_SH		24
#define FWHDR_FWHDR_VER_MSK         0xff

/* dword4 */
#define FWHDR_MONTH_SH			0
#define FWHDR_MONTH_MSK             0xff
#define FWHDR_DATE_SH			8
#define FWHDR_DATE_MSK              0xff
#define FWHDR_HOUR_SH			16
#define FWHDR_HOUR_MSK              0xff
#define FWHDR_MIN_SH			24
#define FWHDR_MIN_MSK               0xff

/* dword5 */
#define FWHDR_YEAR_SH			0
#define FWHDR_YEAR_MSK              0xffff

/* dword6 */
#define FWHDR_IMAGEFROM_SH          0
#define FWHDR_IMAGEFROM_MSK         0x3
#define FWHDR_BOOTFROM_SH           4
#define FWHDR_BOOTFROM_MSK          0x3
#define FWHDR_FW_TYPE               BIT(7)
#define FWHDR_SEC_NUM_SH            8
#define FWHDR_SEC_NUM_MSK           0xff
#define FWHDR_HCI_TYPE_SH		16
#define FWHDR_HCI_TYPE_MSK          0xf
#define FWHDR_NET_TYPE_SH		20
#define FWHDR_NET_TYPE_MSK          0xf

/* dword7 */
#define FWHDR_FW_PART_SZ_SH         0
#define FWHDR_FW_PART_SZ_MSK        0xffff
#define FWHDR_CMD_VER_SH            24
#define FWHDR_CMD_VER_MSK           0xff

/* === Section header === */
/* dword0 */
#define SECTION_INFO_SEC_DL_ADDR_SH      0
#define SECTION_INFO_SEC_DL_ADDR_MSK     0xffffffff

/* dword1 */
#define SECTION_INFO_SEC_SIZE_SH         0
#define SECTION_INFO_SEC_SIZE_MSK        0xffffff
#define SECTION_INFO_SECTIONTYPE_SH      24
#define SECTION_INFO_SECTIONTYPE_MSK     0xf
#define SECTION_INFO_CHECKSUM            BIT(28)
#define SECTION_INFO_REDL                BIT(29)

u32 mac_fwdl(struct mac_ax_adapter *adapter, u8 *fw, u32 len);
u32 mac_romdl(struct mac_ax_adapter *adapter, u8 *rom, u32 romaddr, u32 len);

#endif
