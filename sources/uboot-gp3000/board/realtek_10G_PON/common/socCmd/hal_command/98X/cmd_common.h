/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	cmd_common.h
	
Abstract:
	Define 98E test common command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/


/* Minimum and maximum values a `signed long int' can hold.
   (Same as `int').  */
#ifndef __LONG_MAX__
#if defined (__alpha__) || (defined (__sparc__) && defined(__arch64__)) || defined (__sparcv9) || defined (__s390x__)
#define __LONG_MAX__ 9223372036854775807L
#else
#define __LONG_MAX__ 2147483647L
#endif /* __alpha__ || sparc64 */
#endif
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX-1)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#ifndef __LONG_LONG_MAX__
#define __LONG_LONG_MAX__ 9223372036854775807LL
#endif

u32
Strtoul(
    IN  const u8 *nptr,
    IN  u8 **endptr,
    IN  u32 base
);

u32
CmdRamHelp(
    IN  u16 argc, 
    IN  u8  *argv[] 
);

u32
CmdDumpByte(
    IN  u16 argc, 
    IN  u8  *argv[] 
);

u32 
CmdDumpHelfWord(
    IN  u16 argc, 
    IN  u8  *argv[]
);

u32 
CmdDumpWord(
    IN  u16 argc, 
    IN  u8  *argv[]
);

u32 
CmdWriteByte(
    IN  u16 argc, 
    IN  u8  *argv[] 
);

u32 
CmdWriteHword( 
    IN u16 argc,
    IN u8 *argv[]
);

u32 
CmdWriteWord(
    IN  u16 argc, 
    IN  u8  *argv[] 
);

u32
CmdSOCTest(
    IN  u16 argc, 
    IN  u8  *argv[] 
);


