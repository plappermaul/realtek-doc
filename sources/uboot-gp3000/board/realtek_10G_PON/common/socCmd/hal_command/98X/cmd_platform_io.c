/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_monitor.c
	
Abstract:
	Define 98E test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/


#include <common.h>
#include <command.h>
#include "basic_types.h"
#include "cmd_common.h"


static int Cmddb(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdDumpByte(argc - 1,(u8**)(argv+1) );
	return CMD_RET_SUCCESS;
}

static int CmdDFW(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdDumpHelfWord(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdDW(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdDumpWord(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdWB(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdWriteByte(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdWHW(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdWriteHword(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}

static int CmdWW(cmd_tbl_t * cmdtp, int flag, int argc, char * argv[])
{
    CmdWriteWord(argc - 1,(u8**)(argv+1) );
    return CMD_RET_SUCCESS;
}


U_BOOT_CMD(db, 5, 0,	Cmddb,
        "DB <Address, Hex> <Len, Dec>:",
        "Dump memory byte or Read Hw byte register"
);


U_BOOT_CMD(dhw, 5, 0,	CmdDFW,
        "DHW <Address, Hex> <Len, Dec>:",
        "Dump memory helf-word or Read Hw helf-word register;\n" 
        "Unit: 4Bytes"
);



U_BOOT_CMD(dw, 5, 0,	CmdDW,
        "DW <Address, Hex> <Len, Dec>:",
        "Dump memory word or Read Hw word register; \n" 
        "Unit: 4Bytes"
);



U_BOOT_CMD(eb, 5, 0,	CmdWB,
        "EB <Address, Hex> <Value, Hex>:",
        " Write memory byte or Write Hw byte register \n"
        "Can write more word at the same time \n" 
        "Ex: EB Address Value0 Value1"
);


U_BOOT_CMD(ehw, 5, 0,	CmdWHW,
	    "EHW <Address, Hex> <Value, Hex>:",
        "Write memory word or Write Hw half word register \n"
        "Can write more word at the same time \n" 
        "Ex: EHW Address Value0 Value1"
);


U_BOOT_CMD(ew, 5, 0,	CmdWW,
	    "EW <Address, Hex> <Value, Hex>:",
        "Write memory word or Write Hw word register \n"
        "Can write more word at the same time \n" 
        "Ex: EW Address Value0 Value1"
);



