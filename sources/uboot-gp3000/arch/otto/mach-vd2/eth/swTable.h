/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002
* All rights reserved.
*
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/swTable.h,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: Switch core polling mode NIC header file.
*

*
* ---------------------------------------------------------------
*/

#ifndef _SWNIC_TABLE_H
#define _SWNIC_TABLE_H

void tableAccessForeword(uint32_t, uint32_t, void *);
int swTable_addEntry(uint32_t tableType, uint32_t eidx, void *entryContent_P);
int swTable_modifyEntry(uint32_t tableType, uint32_t eidx, void *entryContent_P);
int swTable_forceAddEntry(uint32_t tableType, uint32_t eidx, void *entryContent_P);
int swTable_readEntry(uint32_t tableType, uint32_t eidx, void *entryContent_P);

#endif /* _SWNIC_H */
