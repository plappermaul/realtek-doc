/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include <common.h>
#include <command.h>
#include <basic_types.h>
#include "cmd_common.h"
#include "cmd_pcie.h"
//#include "dw_pcie_base_test.h"
#include "peripheral.h"

extern void PCIeTestApp(IN void *PCIeTestData);

u32
CmdPCIeTest(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32  PCIeTestData[4];
    
    if (argc<4)
    {
        printf("Wrong argument number!\r\n");
        return _FALSE;
    }

    PCIeTestData[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10); // item
    PCIeTestData[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10); // sub-item
    PCIeTestData[2] = Strtoul((const u8*)(argv[2]), (u8 **)NULL, 10); // PCIe Index
    PCIeTestData[3] = Strtoul((const u8*)(argv[3]), (u8 **)NULL, 10); // loop count

    PCIeTestApp(PCIeTestData);

    return _TRUE;
}

