


/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_timer.h
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/

#ifndef _CMD_Timer_H_
#define _CMD_Timer_H_

u32
CmdTimerTest(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
CmdWatchdogTest(
    IN  u16 argc,
    IN  u8  *argv[]
);

u32
CmdChangeCpuFreqTest(
    IN  u16 argc,
    IN  u8  *argv[]
);

#endif // #ifndef _CMD_Timer_H_

