/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Cmd_common.c
	
Abstract:
	Define 98E common cmd
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2014-05-19 Eric            Create.	
--*/

#include <common.h>
#include <command.h>
#include <basic_types.h>
//#include "cmd_monitor.h"
#include "cmd_common.h"


u32 Isspace(char ch)
{
    return (u32)((ch - 9) < 5u  ||  ch == ' ');
}

u32
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



void 
DumpForOneBytes(
    IN  u8 *pData, 
    IN  u8   Len
)
{
	u8 *pSbuf = pData;	
	s8 Length=Len;

	s8 LineIndex=0,BytesIndex,Offset;
	printf("\r [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n" );

	while( LineIndex< Length)
	{		
			printf("%08X: ", (pSbuf+LineIndex) );

			if(LineIndex+16 < Length)
				Offset=16;
			else			
				Offset=Length-LineIndex;
			

			for(BytesIndex=0; BytesIndex<Offset; BytesIndex++)
				printf("%02x ", pSbuf[LineIndex+BytesIndex]);	

			for(BytesIndex=0;BytesIndex<16-Offset;BytesIndex++)	//a last line
    			printf("   ");


			printf("    ");		//between byte and char
			
			for(BytesIndex=0;  BytesIndex<Offset; BytesIndex++) {
                
				if( ' ' <= pSbuf[LineIndex+BytesIndex]  && pSbuf[LineIndex+BytesIndex] <= '~')
					printf("%c", pSbuf[LineIndex+BytesIndex]);
				else
					printf(".");
			}
            
			printf("\n\r");
			LineIndex += 16;
	}
}



u32
CmdDumpByte(
    IN  u16 argc, 
    IN  u8  *argv[] 
)
{
	
	u32 Src;
	u32 Len;

	if(argc<1)
	{	printf("Wrong argument number!\r\n");
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

u32 
CmdDumpHelfWord(
    IN  u16 argc, 
    IN  u8  *argv[]
)
{
	u32 Src;
	u32 Len,LenIndex;

	if(argc<1)
	{	printf("Wrong argument number!\r\n");
		return _FALSE;
	}
	
	if(argv[0])	{
        Src= Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
	}
	else
	{	printf("Wrong argument number!\r\n");
		return _FALSE;		
	}
				
	if(!argv[1])
		Len = 1;
	else
    	Len = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);	
    
	while ( (Src) & 0x01)
		Src++;

	for(LenIndex=0; LenIndex< Len ; LenIndex+=4,Src+=16)
	{	
		printf("%08X:	%04X	%04X	%04X    %04X    %04X	%04X	%04X    %04X\n",
		Src, *(u16 *)(Src), *(u16 *)(Src+2), 
		*(u16 *)(Src+4), *(u16 *)(Src+6),
		*(u16 *)(Src+8), *(u16 *)(Src+10),
		*(u16 *)(Src+12), *(u16 *)(Src+14));
	}
    return _TRUE;

}


u32 
CmdDumpWord(
    IN  u16 argc, 
    IN  u8  *argv[]
)
{
	u32 Src;
	u32 Len,LenIndex;

	if(argc<1)
	{	printf("Wrong argument number!\r\n");
		return _FALSE;
	}

	if(argv[0])	{
        Src= Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);
	}
	else
	{	printf("Wrong argument number!\r\n");
		return _FALSE;		
	}

			
	if(!argv[1])
		Len = 1;
	else
    	Len = Strtoul((const u8*)(argv[1]), (u8 **)NULL, 10);	
    
	while ( (Src) & 0x03)
		Src++;


	for(LenIndex=0; LenIndex< Len ; LenIndex+=4,Src+=16)
	{	
		printf("%08X:	%08X	%08X	%08X	%08X\n",
		Src, *(u32 *)(Src), *(u32 *)(Src+4), 
		*(u32 *)(Src+8), *(u32 *)(Src+12));
	}
    
    return _TRUE;

}


u32 
CmdWriteByte(
    IN  u16 argc, 
    IN  u8  *argv[] 
)
{
	u32 src;
	u8 value,i;

	src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);		


	for(i=0;i<argc-1;i++,src++)
	{
		value= Strtoul((const u8*)(argv[i+1]), (u8 **)NULL, 16);	
        printf("addr: 0x%x; value:0x%x\n", src, value);        
		*(volatile u8 *)(src) = value;        
	}

	return 0;
}

u32 
CmdWriteHword( 
    IN u16 argc,
    IN u8 *argv[] )
{
	
	u32 src;
	u16 value,i;
	
	src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);		
	
	src &= 0xfffffffe;	

	for(i=0;i<argc-1;i++,src+=2)
	{
		value= Strtoul((const u8*)(argv[i+1]), (u8 **)NULL, 16);	
        printf("addr: 0x%x; value:0x%x\n", src, value);
		*(volatile u16 *)(src) = value;
	}
	
}


u32 
CmdWriteWord(
    IN  u16 argc, 
    IN  u8  *argv[] 
)
{
	u32 src;
	u32 value,parameterIndex;
	
	src = Strtoul((const u8*)(argv[0]), (u8 **)NULL, 16);		
	while ( (src) & 0x03)
		src++;

	for(parameterIndex=0;parameterIndex<argc-1;parameterIndex++,src+=4)
	{
		value= Strtoul((const u8*)(argv[parameterIndex+1]), (u8 **)NULL, 16);
        printf("addr: 0x%x; value:0x%x\n", src, value);
		*(volatile u32 *)(src) = value;
	}

        
	return 0;
}



