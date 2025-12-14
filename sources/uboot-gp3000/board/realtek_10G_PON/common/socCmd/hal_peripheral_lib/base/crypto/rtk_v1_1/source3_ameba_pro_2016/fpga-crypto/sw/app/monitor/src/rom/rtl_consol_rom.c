/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtl8195a.h"
#include <stdarg.h>
#include "rtl_consol.h"
#include "strproc.h"
//#include "osdep_api.h"

//MON_RAM_BSS_SECTION
extern volatile UART_LOG_CTL    UartLogCtl;
//MON_RAM_BSS_SECTION
extern volatile UART_LOG_CTL    *pUartLogCtl;
//MON_RAM_BSS_SECTION
extern u8                       *ArgvArray[MAX_ARGV];
//MON_RAM_BSS_SECTION
extern UART_LOG_BUF              UartLogBuf;

extern _LONG_CALL_ROM_ u32 HalDelayUs(u32 us);

#ifdef CONFIG_UART_LOG_HISTORY
//MON_RAM_BSS_SECTION
extern u8  UartLogHistoryBuf[UART_LOG_HISTORY_LEN][UART_LOG_CMD_BUFLEN];
#endif

u8
GetArgc(
    IN  const   u8  *string
);


u8**
GetArgv(
    IN  const   u8  *string
);


u8*
StrUpr(
    IN  u8  *string
);

_LONG_CALL_ u8
prvStrCmp(
    IN  const   u8  *string1,
    IN  const   u8  *string2
);



u8
UartLogCmdChk(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
);


VOID
ArrayInitialize(
    IN  u8  *pArrayToInit,
    IN  u8  ArrayLen,
    IN  u8  InitValue
);



VOID
UartLogIrqHandle
(
    VOID * Data
);
//=================================================

//1 ToDo: Need Modify
MON_ROM_TEXT_SECTION
u32 Isspace(char ch)
{
    return (u32)((ch - 9) < 5u  ||  ch == ' ');
}

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

MON_ROM_TEXT_SECTION
_LONG_CALL_ u32
Strtoul(
    IN  const u8 *nptr,
    IN  u8 **endptr,
    IN  u32 base
)
{
    u32 v=0;

    while(Isspace(*nptr)) ++nptr;
    if (*nptr == '+') ++nptr;
    if (base==16 && nptr[0]=='0') goto skip0x;
    if (!base) {
        if (*nptr=='0') {
            base=8;
skip0x:
            if (nptr[1]=='x'||nptr[1]=='X') {
                nptr+=2;
                base=16;
            }
        } else
            base=10;
    }
    while(*nptr) {
        register u8 c=*nptr;
        c=(c>='a'?c-'a'+10:c>='A'?c-'A'+10:c<='9'?c-'0':0xff);
        if (c>=base) break;
        {
            register u32 w=v*base;
            if (w<v) {
                //errno=ERANGE;
                return ULONG_MAX;
            }
            v=w+c;
        }
        ++nptr;
    }
    if (endptr) *endptr=(u8 *)nptr;
    //errno=0;    /* in case v==ULONG_MAX, ugh! */
    return v;
}



//======================================================
//<Function>:  ArrayInitialize
//<Usage   >:  This function is to initialize an array with a given value.
//<Argus    >:  pArrayToInit --> pointer to an array
//                   ArrayLen      --> number of array elements
//                   InitValue     --> initial value for the array
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ VOID
ArrayInitialize(
    IN  u8  *pArrayToInit,
    IN  u8  ArrayLen,
    IN  u8  InitValue
)
{
    u8  ItemCount =0;

    for (ItemCount=0; ItemCount<ArrayLen; ItemCount++) {
        *(pArrayToInit+ItemCount) = InitValue;
    }

}


//======================================================
//<Function>:  GetArgc
//<Usage   >:  This function is to calcute how many parameters in the cmd string
//<Argus    >:  string      --> pointer to an input cmd string
//<Return   >:  number of parameters in the string.(****except the cmd itself****)
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ u8
GetArgc(
    IN  const   u8  *string
)
{
    u16 argc;
    u8  *pStr;

    argc = 0;
    pStr = (u8 *)string;

    while(*pStr) {
        if ((*pStr != ' ') && (*pStr)) {
            argc++;
            while ((*pStr != ' ') && (*pStr)) {
                pStr++;
            }

            continue;
        }

        pStr++;
    }

    if (argc >= MAX_ARGV) {
        argc = MAX_ARGV - 1;
    }

    return argc;
}

//======================================================
//<Function>:  GetArgv
//<Usage   >:  This function is to transfer a cmd string to a char array.
//<Argus    >:  string      --> pointer to an input cmd string
//<Return   >:  pointer to a char array
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ u8**
GetArgv(
    IN  const   u8  *string
)
{
    u8  ArgvCount = 0;
    u8  *pStr;

    ArrayInitialize((u8*)ArgvArray, MAX_ARGV*sizeof(char *), 0);
    pStr = (u8 *)string;

    while(*pStr) {
        ArgvArray[ArgvCount] = pStr;
        while((*pStr != ' ') && (*pStr)) {
            pStr++;
        }

        *(pStr++) = '\0';

        while((*pStr == ' ') && (*pStr)) {
            pStr++;
        }

        ArgvCount++;

        if (ArgvCount == MAX_ARGV) {
            break;
        }
    }

    return (u8 **)&ArgvArray;
}




//======================================================
//<Function>:  UartLogCmdExecute
//<Usage   >:  This function is to parse Uart-Log cmds. If there's a matched
//                      one, it goes to execute that.
//<Argus    >:  pointer to Uart-Log cmd temp buffer
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ VOID
UartLogCmdExecute(
    IN  PUART_LOG_CTL   pUartLogCtlExe
)
{
    u8  CmdCnt = 0;
    u8  argc = 0;
    u8  **argv;
    u32  CmdNum;
    PUART_LOG_BUF   pUartLogBuf = pUartLogCtlExe->pTmpLogBuf;
    PCOMMAND_TABLE pCmdTbl = pUartLogCtlExe->pCmdTbl;

    argc = GetArgc((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
    argv = GetArgv((const u8*)&((*pUartLogBuf).UARTLogBuf[0]));
    CmdNum = pUartLogCtlExe->CmdTblSz;

    if (argc > 0) {
        StrUpr(argv[0]);

        for (CmdCnt = 0; CmdCnt< CmdNum; CmdCnt++) {
            if ((prvStrCmp(argv[0], pCmdTbl[CmdCnt].cmd))==0) {
                pCmdTbl[CmdCnt].func((argc-1) , (argv+1));
                ArrayInitialize(argv[0], sizeof(argv[0]) ,0);
                break;
            }
        }
    }

    (*pUartLogBuf).BufCount = 0;
    ArrayInitialize(&(*pUartLogBuf).UARTLogBuf[0], UART_LOG_CMD_BUFLEN, '\0');
}


//======================================================
//<Function>:  UartLogShowBackSpace
//<Usage   >:  To dispaly backspace on the target interface.
//<Argus    >:  BackLen      --> backspace number to show
//                   EchoFlag     --> a control flag to show msg or not.
//                   pfINPUT      --> func pointer to the display function
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ VOID
UartLogShowBackSpace(
    IN  u8  BackLen,
    IN  u8  EchoFlag,
    IN  VOID *pfINPUT
)
{
    u8  BackCnt;
    ECHOFUNC pfEcho;
    pfEcho = (ECHOFUNC)pfINPUT;

    for(BackCnt=0; BackCnt<BackLen; BackCnt++) {
        if (EchoFlag) {
            pfEcho((u8*)"%c",KB_ASCII_BS);
            pfEcho((u8*)" ");
            pfEcho((u8*)"%c",KB_ASCII_BS);
        }
    }
}

#ifdef CONFIG_UART_LOG_HISTORY
//======================================================
//<Function>:  UartLogRecallOldCmd
//<Usage   >:  To retrieve the old cmd and may show it on the target interface.
//<Argus    >:  pTmpBuf      --> cmd temp buffer, the retrieved cmd will be stored in
//                                            it.
//                   OldBuf         --> pointer to an cmd buffer.
//                   EchoFlag      --> a control flag to show msg or not.
//                   pfINPUT       --> func pointer to the display function
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ VOID
UartLogRecallOldCmd(
    IN  PUART_LOG_BUF  pTmpBuf,
    IN  u8  *OldBuf,
    IN  u8  EchoFlag,
    IN  VOID *pfINPUT
)
{

    ECHOFUNC pfEcho;
    pfEcho = (ECHOFUNC)pfINPUT;

    prvStrCpy(&((*pTmpBuf).UARTLogBuf[0]), OldBuf);
    (*pTmpBuf).BufCount = prvStrLen(&(*pTmpBuf).UARTLogBuf[0]);

    if (EchoFlag) {
        pfEcho(&((*pTmpBuf).UARTLogBuf[0]));
    }

}

//======================================================
//<Function>:  UartLogHistoryCmd
//<Usage   >:  To deal with history-cmds, get the next/previous cmd and show it.
//<Argus    >:  RevData         --> a received data.
//                   prvUartLogCtl  --> pointer to UART_LOG_CTL
//                   EchoFlag        --> a control flag to show msg or not.
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ VOID
UartLogHistoryCmd(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
)
{
    ECHOFUNC pfEcho;
    pfEcho = (ECHOFUNC)(*prvUartLogCtl).pfINPUT;

    if (RevData == 'A') {
        if ((*prvUartLogCtl).CRSTS == 0) {
            if ((*prvUartLogCtl).RevdNo > 0) {
                if ((*prvUartLogCtl).SeeIdx == 0) {
                    (*prvUartLogCtl).SeeIdx = (*prvUartLogCtl).RevdNo - 1;
                } else {
                    (*prvUartLogCtl).SeeIdx--;
                }
            }
        }

        UartLogShowBackSpace((*(*prvUartLogCtl).pTmpLogBuf).BufCount, EchoFlag, pfEcho);

        if ((*prvUartLogCtl).RevdNo > 0) {
            UartLogRecallOldCmd((*prvUartLogCtl).pTmpLogBuf , (u8*)&(*prvUartLogCtl).pHistoryBuf[(*prvUartLogCtl).SeeIdx],
                                1, (VOID*)pfEcho);

        }

        (*prvUartLogCtl).CRSTS = 0;

    } else if (RevData == 'B') {

        if ((*prvUartLogCtl).RevdNo > 0) {
            if ((*prvUartLogCtl).SeeIdx == ((*prvUartLogCtl).RevdNo-1)) {
                (*prvUartLogCtl).SeeIdx = 0;
            } else {
                (*prvUartLogCtl).SeeIdx++;
            }
        }

        UartLogShowBackSpace(((*(*prvUartLogCtl).pTmpLogBuf).BufCount), 1, pfEcho);

        if ((*prvUartLogCtl).RevdNo > 0) {
            UartLogRecallOldCmd((*prvUartLogCtl).pTmpLogBuf, (u8*)&(*prvUartLogCtl).pHistoryBuf[(*prvUartLogCtl).SeeIdx],
                                1, (VOID*)pfEcho);
        }

        (*prvUartLogCtl).CRSTS = 0;

    }
}
#endif

//======================================================
//<Function>:  UartLogCmdChk
//<Usage   >:  To deal with Uart-Log input characters
//<Argus    >:  RevData         --> a received data.
//                   prvUartLogCtl  --> pointer to UART_LOG_CTL
//                   EchoFlag        --> a control flag to show msg or not.
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ROM_ u8
UartLogCmdChk(
    IN  u8  RevData,
    IN  UART_LOG_CTL    *prvUartLogCtl,
    IN  u8  EchoFlag
)
{
    u8  RTNSTS = 0;
    u8  SpaceCount = 0;

    //Cast the input function pointer to the actual echo function pointer type-----
    ECHOFUNC pfEcho;
    pfEcho = (ECHOFUNC)(*prvUartLogCtl).pfINPUT;
    //----------------------------------------------------------------

    if (((*(*prvUartLogCtl).pTmpLogBuf).BufCount<(UART_LOG_CMD_BUFLEN-1)) ||
            (((*(*prvUartLogCtl).pTmpLogBuf).BufCount==(UART_LOG_CMD_BUFLEN-1)) &&
             ((RevData == KB_ASCII_CR)||(RevData == '\0')||
              (RevData == KB_ASCII_BS) || (RevData == KB_ASCII_BS_7F)))) {

        if (RevData == 0xFF) {  //invlid char, ignore it
            RTNSTS = 1;
        }

        if (RevData == KB_ASCII_LF) {
            RTNSTS = 1;
        } else if ((RevData == KB_ASCII_CR)||(RevData == '\0')) {
            RTNSTS = 2;
            (*prvUartLogCtl).CRSTS = 1;
            (*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = '\0';
            if (EchoFlag) {
                pfEcho((u8*)"\r\n");
            }

            if((*(*prvUartLogCtl).pTmpLogBuf).BufCount != 0) {
#ifdef CONFIG_UART_LOG_HISTORY
                prvStrCpy((u8*)&((*prvUartLogCtl).pHistoryBuf[(*prvUartLogCtl).NewIdx]),
                          (const u8*)&((*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf));

                ((*prvUartLogCtl).RevdNo < UART_LOG_HISTORY_LEN) ?
                ((*prvUartLogCtl).RevdNo++) : ((*prvUartLogCtl).RevdNo = UART_LOG_HISTORY_LEN);

                (*prvUartLogCtl).SeeIdx = (*prvUartLogCtl).NewIdx;
                ((*prvUartLogCtl).NewIdx < (UART_LOG_HISTORY_LEN-1)) ?
                ((*prvUartLogCtl).NewIdx++) : ((*prvUartLogCtl).NewIdx = 0);


#endif

            }
        } else if ((RevData == KB_ASCII_BS) || (RevData == KB_ASCII_BS_7F)) {
            RTNSTS = 1;

            if ((*(*prvUartLogCtl).pTmpLogBuf).BufCount > 0) {
                (*(*prvUartLogCtl).pTmpLogBuf).BufCount--;

                UartLogShowBackSpace(1,1,pfEcho);
            }
        } else if (RevData == KB_ASCII_TAB) { //the TAB is fixed to be one space(' ')
            RTNSTS = 1;
            for(SpaceCount=0; SpaceCount< KB_SPACENO_TAB; SpaceCount++) {
                (*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = ' ';
                (*(*prvUartLogCtl).pTmpLogBuf).BufCount++;
            }

            if (EchoFlag) {
                pfEcho((u8 *)" ");
            }

        } else {
            RTNSTS = 1;
            if (EchoFlag) {
                pfEcho((u8*)"%c",RevData);
            }

            (*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = RevData;
            (*(*prvUartLogCtl).pTmpLogBuf).BufCount++;
        }
    } else if ((*(*prvUartLogCtl).pTmpLogBuf).BufCount==(UART_LOG_CMD_BUFLEN-1)) {
        RTNSTS = 1;

        pfEcho((u8*)"\n\r <<<Too long cmd string.>>> \n");
        (*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[(*(*prvUartLogCtl).pTmpLogBuf).BufCount] = '\0';
        pfEcho(&(*(*prvUartLogCtl).pTmpLogBuf).UARTLogBuf[0]);
    }

    return RTNSTS;
}


//======================================================
//<Function>:  UartLogIrqHandle
//<Usage   >:  To deal with Uart-Log RX IRQ
//<Argus    >:  VOID
//<Return   >:  VOID
//<Notes    >:  NA
//======================================================
MON_ROM_TEXT_SECTION
_LONG_CALL_ VOID
UartLogIrqHandle
(
    VOID * Data
)
{
    u8      UartReceiveData = 0;
    //For Test
    BOOL    PullMode = _FALSE;

    u32 IrqEn = DiagGetIsrEnReg();

    DiagSetIsrEnReg(0);

    UartReceiveData = DiagGetChar(PullMode);

    //KB_ESC chk is for cmd history, it's a special case here.
    if (UartReceiveData == KB_ASCII_ESC) {
        //4 Esc detection is only valid in the first stage of boot sequence (few seconds)
        if (pUartLogCtl->ExecuteEsc != _TRUE) {
            pUartLogCtl->ExecuteEsc = _TRUE;
            (*pUartLogCtl).EscSTS = 0;
        } else {
            //4 the input commands are valid only when the task is ready to execute commands
            if ((pUartLogCtl->BootRdy == 1)) {
                if ((*pUartLogCtl).EscSTS==0) {
                    (*pUartLogCtl).EscSTS = 1;
                }
            } else {
                (*pUartLogCtl).EscSTS = 0;
            }
        }
    } else if ((*pUartLogCtl).EscSTS==1) {
        if (UartReceiveData != KB_ASCII_LBRKT) {
            (*pUartLogCtl).EscSTS = 0;
        } else {
            (*pUartLogCtl).EscSTS = 2;
        }
    }

    else {
        if ((*pUartLogCtl).EscSTS==2) {
            (*pUartLogCtl).EscSTS = 0;
#ifdef CONFIG_UART_LOG_HISTORY
            if ((UartReceiveData=='A')|| UartReceiveData=='B') {
                UartLogHistoryCmd(UartReceiveData,(UART_LOG_CTL *)pUartLogCtl,1);
            }
#endif
        } else {
            if (UartLogCmdChk(UartReceiveData,(UART_LOG_CTL *)pUartLogCtl,1)==2) {
                //4 check UartLog buffer to prevent from incorrect access
                if (pUartLogCtl->pTmpLogBuf != NULL) {
                    pUartLogCtl->ExecuteCmd = _TRUE;

                } else {
                    ArrayInitialize((u8 *)pUartLogCtl->pTmpLogBuf->UARTLogBuf, UART_LOG_CMD_BUFLEN, '\0');
                }
            }
        }
    }
    DiagSetIsrEnReg(IrqEn);

}


MON_ROM_TEXT_SECTION
_LONG_CALL_ VOID
RtlConsolInit(
    IN  u32     Boot,
    IN  u32     TBLSz,
    IN  VOID    *pTBL
)
{
    UartLogBuf.BufCount = 0;
    ArrayInitialize(&UartLogBuf.UARTLogBuf[0],UART_LOG_CMD_BUFLEN,'\0');
    pUartLogCtl = &UartLogCtl;

    pUartLogCtl->NewIdx = 0;
    pUartLogCtl->SeeIdx = 0;
    pUartLogCtl->RevdNo = 0;
    pUartLogCtl->EscSTS = 0;
    pUartLogCtl->BootRdy = 0;
    pUartLogCtl->pTmpLogBuf = &UartLogBuf;
#ifdef CONFIG_UART_LOG_HISTORY
    pUartLogCtl->CRSTS = 0;
    pUartLogCtl->pHistoryBuf = &UartLogHistoryBuf[0];
#endif
    pUartLogCtl->pfINPUT = &DiagPrintf;
    pUartLogCtl->pCmdTbl = (PCOMMAND_TABLE) pTBL;
    pUartLogCtl->CmdTblSz = TBLSz;
#ifdef CONFIG_KERNEL
    pUartLogCtl->TaskRdy = 0;
#endif
    //executing boot sequence
    if (Boot == ROM_STAGE) {
        pUartLogCtl->ExecuteCmd = _FALSE;
        pUartLogCtl->ExecuteEsc = _FALSE;
    } else {
        pUartLogCtl->ExecuteCmd = _FALSE;
        pUartLogCtl->ExecuteEsc= _TRUE;//don't check Esc anymore
#if 0
#if defined(CONFIG_KERNEL)
        /* Create a Semaphone */
        RtlInitSema((_Sema*)&(pUartLogCtl->Sema), 0);
        pUartLogCtl->TaskRdy = 0;
#ifdef PLATFORM_FREERTOS
        if (pdTRUE != xTaskCreate( RtlConsolTaskRam, (const signed char * const)"LOGUART_TASK", 512, NULL, 1, NULL)) {
            DiagPrintf("Create Log UART Task Err!!\n");
        }
#endif
#endif
#endif
    }

    CONSOLE_8195A();
}


MON_ROM_TEXT_SECTION
_LONG_CALL_ VOID
RtlConsolTaskRom(
    VOID *Data
)
{
    //4 Set this for UartLog check cmd history
#ifndef CONFIG_KERNEL
    pUartLogCtl->BootRdy = 1;
#endif
    do {

        if ((pUartLogCtl->ExecuteCmd) == _TRUE) {
            UartLogCmdExecute((PUART_LOG_CTL)pUartLogCtl);
            CONSOLE_8195A();
            pUartLogCtl->ExecuteCmd = _FALSE;
        }
    } while(1);
}


#ifdef CONFIG_BOOT_PROCEDURE

#ifdef CONFIG_TIMER_MODULE
extern _LONG_CALL_ u32 HalDelayUs(u32 us);
#endif

MON_ROM_TEXT_SECTION
_LONG_CALL_ BOOLEAN
RtlExitConsol(
    IN  u32     MaxWaitCount
)
{
    u32 WaitCount = 0;

    do {
        if (WaitCount > MaxWaitCount) {
            return _TRUE;// go back to the normal boot sequence
        }

#ifdef CONFIG_TIMER_MODULE
        HalDelayUs(100);
#endif
//        DBG_8195A("Wait %d\n",WaitCount);
        WaitCount++;

        //4 there is a ESC key input in Boot Sequence check stage
        if (pUartLogCtl->ExecuteEsc == _TRUE) {
            CONSOLE_8195A();

            (*pUartLogCtl).EscSTS = 0;

            break;
        }
    } while(1);

    return _FALSE;

}


MON_ROM_TEXT_SECTION
_LONG_CALL_ VOID
RtlConsolRom(
    IN  u32     MaxWaitCount
)
{

    if (RtlExitConsol(MaxWaitCount)) {
        //Go back to normal boot sequence
        return;
    }

    pUartLogCtl->BootRdy = 1;

    //4 Stay in console stage
    do {
        if ((pUartLogCtl->ExecuteCmd) == _TRUE) {
            UartLogCmdExecute((PUART_LOG_CTL)pUartLogCtl);
            CONSOLE_8195A();
            pUartLogCtl->ExecuteCmd = _FALSE;
        }
    } while(1);
}
#endif
