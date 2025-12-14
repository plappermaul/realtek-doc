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

HAL_ROM_TEXT_SECTION
VOID
DumpForOneBytes(
    IN  u8 *pData,
    IN  u8   Len
)
{
    u8 *pSbuf = pData;
    s8 Length=Len;

    s8 LineIndex=0,BytesIndex,Offset;
    MONITOR_LOG("\r [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n" );

    while( LineIndex< Length) {
        MONITOR_LOG("%08X: ", (pSbuf+LineIndex) );

        if(LineIndex+16 < Length)
            Offset=16;
        else
            Offset=Length-LineIndex;


        for(BytesIndex=0; BytesIndex<Offset; BytesIndex++)
            MONITOR_LOG("%02x ", pSbuf[LineIndex+BytesIndex]);

        for(BytesIndex=0; BytesIndex<16-Offset; BytesIndex++)	//a last line
            MONITOR_LOG("   ");


        MONITOR_LOG("    ");		//between byte and char

        for(BytesIndex=0;  BytesIndex<Offset; BytesIndex++) {

            if( ' ' <= pSbuf[LineIndex+BytesIndex]  && pSbuf[LineIndex+BytesIndex] <= '~')
                MONITOR_LOG("%c", pSbuf[LineIndex+BytesIndex]);
            else
                MONITOR_LOG(".");
        }

        MONITOR_LOG("\n\r");
        LineIndex += 16;
    }

}

