/*
 * (C) Copyright 2000-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 ********************************************************************
 * NOTE: This header file defines an interface to U-Boot. Including
 * this (unmodified) header file in another file is considered normal
 * use of U-Boot, and does *not* fall under the heading of "derived
 * work".
 ********************************************************************
 */

#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#define IMG_PART0         0
#define IMG_PART1         1
#define BDINFO_ENV_SIZE          0x800
#define SYSINFO_ENV_SIZE         0x1000
//#define SYSINFO_ENV_SIZE         CFG_SYSENV_SIZE
#define SYSINFO_VAR_DUALACTPART  "bootpartition"
#define SYSINFO_VAR_DUALFNAME_0  "dualfname0"
#define SYSINFO_VAR_DUALFNAME_1  "dualfname1"
#define SYSINFO_VAR_FLSHERAS     "flsheras"
#define SYSINFO_VAR_PWDRECOV     "pwdrecov"
#define SYSINFO_VAR_FACTDFLT     "factdflt"
#define SYSINFO_VAR_RESETDFLT    "resetdflt"

#define LOADER_TAIL "#-TAIL-#" /* LOADER_TAIL */
#define LOADER_TAIL_CHIP_839X 0x83900000
#define LOADER_TAIL_CHIP_838X 0x83800000
#define LOADER_TAIL_CHIP_930X 0x93000000
#define LOADER_TAIL_CHIP_931X 0x93100000

typedef struct loader_tail_s {
    char     lt_key[8];  /* key string #-TAIL-# */
    uint32_t lt_chip;    /* chip family */
    uint32_t lt_crc;     /* loader image crc */
    uint32_t lt_size;    /* loader image size */
    uint32_t lt_tcrc;    /* loader tail crc */
} loader_tail_t;

#define PARTITION_SIZE(partition) flash_partition_size_ret(partition)
#define PARTITION_ADDR(partition) flash_partition_addr_ret(partition)

#endif	/* __SYSINFO_H__ */
