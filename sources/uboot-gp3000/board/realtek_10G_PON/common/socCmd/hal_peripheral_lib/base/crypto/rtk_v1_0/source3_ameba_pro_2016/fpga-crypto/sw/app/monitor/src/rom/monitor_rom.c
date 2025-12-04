/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "rtl_consol.h"
#include "monitor_lib.h"


u32
CmdRomHelp(
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
CmdWriteWord(
    IN  u16 argc,
    IN  u8  *argv[]
);


#ifdef CONFIG_SPIC_MODULE
u32
CmdSpiFlashTool(
    IN  u16 argc,
    IN  u8  *argv[]
);
#endif



MON_ROM_DATA_SECTION
COMMAND_TABLE   UartLogRomCmdTable[] = {
    {(const u8*)"?",        0, CmdRomHelp,        (const u8*)"\tHELP (?)   : Print this help messag\n"},
    {
        (const u8*)"DB",    2, CmdDumpByte,      (const u8*)"\tDB <Address, Hex> <Len, Dec>: \n"
        "\t\t\t\t Dump memory byte or Read Hw byte register"
    },
    {
        (const u8*)"DHW",   2, CmdDumpHelfWord,  (const u8*)"\tDHW <Address, Hex> <Len, Dec>: \n"
        "\t\t\t\t Dump memory helf-word or Read Hw helf-word register"
    },
    {
        (const u8*)"DW",    2, CmdDumpWord,      (const u8*)"\tDW <Address, Hex> <Len, Dec>: \n"
        "\t\t\t\t Dump memory word or Read Hw word register"
    },
    {
        (const u8*)"EW",       2, CmdWriteWord,     (const u8*)"\tEW <Address, Hex> <Value, Hex>: \n"
        "\t\t\t\t Write memory word or Write Hw word register \n"
        "\t\t\t\t Can write more word at the same time \n"
        "\t\t\t\t Ex: EW Address Value0 Value1"
    },
#ifdef CONFIG_SPIC_MODULE
    {
        (const u8*)"SPICTOOL", 2, CmdSpiFlashTool,  (const u8*)"\tSPICTOOL <Mode, Dec> <BitMode, Dec>: \n"
        "\t\t\t\t Mode = 1: Init SPIC;  BitMode: 0(One)/1(Dual)/2(Quad)\n"
        "\t\t\t\t Mode = 2: Erase Chip \n"
    },
#endif
};




MON_ROM_TEXT_SECTION
u32
GetRomCmdNum(
    VOID
)
{
    return (sizeof(UartLogRomCmdTable)/sizeof(COMMAND_TABLE));
}
//======================================================
//<Function>:  CmdTest
//<Usage   >:  This function is a demo test function.
//<Argus    >:  argc --> number of argus
//                   argv --> pointer to a cmd parameter array
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================


MON_ROM_TEXT_SECTION
u32
CmdRomHelp(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32	LoopINdex ;

    DBG_8195A("----------------- COMMAND MODE HELP ------------------\n");
    for( LoopINdex=0  ; LoopINdex < (sizeof(UartLogRomCmdTable) / sizeof(COMMAND_TABLE)) ; LoopINdex++ ) {
        if( UartLogRomCmdTable[LoopINdex].msg ) {
            DBG_8195A( "%s\n",UartLogRomCmdTable[LoopINdex].msg );
        }
    }
    DBG_8195A("----------------- COMMAND MODE END  ------------------\n");

    return _TRUE ;
}


MON_ROM_TEXT_SECTION
u32
CmdDumpByte(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 Src;
    u32 Len;

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    Src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);

    if(!argv[1])
        Len = 16;
    else
        Len = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    DumpForOneBytes((u8 *)Src,(u8)Len);

    return _TRUE ;
}


MON_ROM_TEXT_SECTION
u32
CmdDumpHelfWord(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 Src;
    u32 Len,LenIndex;

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    if(argv[0])	{
        Src= Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    } else {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    if(!argv[1])
        Len = 1;
    else
        Len = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    while ( (Src) & 0x01)
        Src++;

    for(LenIndex=0; LenIndex< Len ; LenIndex+=4,Src+=16) {
        DBG_8195A("%08X:	%04X	%04X	%04X    %04X    %04X	%04X	%04X    %04X\n",
                  Src, *(u16 *)(Src), *(u16 *)(Src+2),
                  *(u16 *)(Src+4), *(u16 *)(Src+6),
                  *(u16 *)(Src+8), *(u16 *)(Src+10),
                  *(u16 *)(Src+12), *(u16 *)(Src+14));
    }
    return _TRUE;

}



MON_ROM_TEXT_SECTION
u32
CmdDumpWord(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 Src;
    u32 Len,LenIndex;

    if(argc<1) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    if(argv[0])	{
        Src= Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    } else {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }

    if(!argv[1])
        Len = 1;
    else
        Len = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    while ( (Src) & 0x03)
        Src++;

    for(LenIndex=0; LenIndex< Len ; LenIndex+=4,Src+=16) {
#if 0
        DBG_8195A("%08X:	%08X	%08X	%08X	%08X\n",
                  Src, *(u32 *)(Src), *(u32 *)(Src+4),
                  *(u32 *)(Src+8), *(u32 *)(Src+12));
#else
        MONITOR_LOG("\r""%08X:    %08X",
                    Src, *(u32 *)(Src));
        MONITOR_LOG("    %08X",
                    *(u32 *)(Src+4));
        MONITOR_LOG("    %08X",
                    *(u32 *)(Src+8));
        MONITOR_LOG("    %08X\n",
                    *(u32 *)(Src+12));
#endif

    }
    return _TRUE;

}


MON_ROM_TEXT_SECTION
u32
CmdWriteByte(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 src;
    u8 value,i;

    src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);


    for(i=0; i<argc-1; i++,src++) {
        value= Strtoul((const u8*)(argv[i+1]), (u8 **)NULL, 16);
        *(volatile u8 *)(src) = value;
    }

    return 0;
}


MON_ROM_TEXT_SECTION
u32
CmdWriteWord(
    IN  u16 argc,
    IN  u8  *argv[]
)
{

    u32 Src;
    u32 Value,ParameterIndex;

    Src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
    while ( (Src) & 0x03)
        Src++;

    for(ParameterIndex=0; ParameterIndex<argc-1; ParameterIndex++,Src+=4) {
        Value= Strtoul((const u8*)(argv[ParameterIndex+1]), (u8 **)NULL, 16);
        DBG_8195A("addr: 0x%x; value:0x%x\n", Src, Value);
        *(volatile u32 *)(Src) = Value;
    }

    return 0;
}

#ifdef CONFIG_SPIC_MODULE
extern VOID SpiFlashApp(VOID *Data);

MON_ROM_TEXT_SECTION
u32
CmdSpiFlashTool(
    IN  u16 argc,
    IN  u8  *argv[]
)
{
    u32 Cmd[2];

    if(argc<2) {
        DBG_8195A("Wrong argument number!\r\n");
        return _FALSE;
    }


    Cmd[0] = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 10);
    Cmd[1] = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);

    SpiFlashApp((VOID*)Cmd);

    return _TRUE;
}
#endif

