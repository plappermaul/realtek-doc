

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_crypto.c
	
Abstract:
	Define 96F test command
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2015-01-20 Xu Junwei       Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_crypto.h"

extern int rtl_crypto_test(IN u16 argc, IN u8 *argv[]);

u32
CmdCRYPTOTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    return rtl_crypto_test(argc, argv);
}


