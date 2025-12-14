
/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_monitor.h
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/


#ifndef _CMD_PLATFORM_IO_H_
#define _CMD_PLATFORM_IO_H_


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
CmdWriteWord(
    IN  u16 argc, 
    IN  u8  *argv[] 
);

#endif //#ifndef _CMD_PLATFORM_IO_H_
