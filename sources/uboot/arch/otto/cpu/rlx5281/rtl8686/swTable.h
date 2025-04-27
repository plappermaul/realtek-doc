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
#define CONFIG_RTL865XC 1

//#include <rtl_types.h>
//#include <rtl_errno.h>
//#include <rtl8650/asicregs.h>


void tableAccessForeword(uint32, uint32, void *);
int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 swTable_modifyEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 swTable_forceAddEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P);




#endif /* _SWNIC_H */
