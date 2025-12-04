/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_gdma.h
	
Abstract:
	Define 96F test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2015-11-13 Carl            Create.	
--*/

#ifndef _CMD_GDMA_H_
#define _CMD_GDMA_H_

u32
CmdGdmaTest(
    IN  u16 argc,
    IN  u8  *argv[]
);

#endif // #ifndef _CMD_GDMA_H_