#include "gdma_utils.h"
#include "gdma.h"
#include "asicregs.h"

char stringToInt(char* s)
{
	char value;

	value = 0;
	while(*s != 0){
        	if ((*s >= '0') && ( *s <= '9')){
			value = value*10 + (*s - '0');
		}
		s++;
	}
        return value;
}

typedef uint32          memaddr;
void memDump (void *start, uint32 size, int8 * strHeader)
{
        int32 row, column, index, index2, max;
        uint32 buffer[5];
        uint8 *buf, *line, ascii[17];
        int8 empty = ' ';

        if(!start ||(size==0))
                return;
        line = (uint8*)start;

        /*
        **16 bytes per line
        ***/
        if (strHeader)
                rtlglue_printf ("%s", strHeader);
        column = size % 16;
        row = (size / 16) + 1;
        for (index = 0; index < row; index++, line += 16)
        {

                /* for un-alignment access */
                buffer[0] = ntohl( READ_MEM32( (((uint32)line)&~3)+ 0 ) );
                buffer[1] = ntohl( READ_MEM32( (((uint32)line)&~3)+ 4 ) );
                buffer[2] = ntohl( READ_MEM32( (((uint32)line)&~3)+ 8 ) );
                buffer[3] = ntohl( READ_MEM32( (((uint32)line)&~3)+12 ) );
                buffer[4] = ntohl( READ_MEM32( (((uint32)line)&~3)+16 ) );
                buf = ((uint8*)buffer) + (((uint32)line)&3);

                memset (ascii, 0, 17);

                max = (index == row - 1) ? column : 16;
                if ( max==0 ) break; /* If we need not dump this line, break it. */

                rtlglue_printf ("\n%08x ", (memaddr) line);

                for (index2 = 0; index2 < max; index2++)
                {
                        if (index2 == 8)
                        rtlglue_printf ("  ");
                        rtlglue_printf ("%02x ", (uint8) buf[index2]);
                        ascii[index2] = ((uint8) buf[index2] < 32) ? empty : buf[index2];
                }

                if (max != 16)
                {
                        if (max < 8)
                                rtlglue_printf ("  ");
                        for (index2 = 16 - max; index2 > 0; index2--)
                                rtlglue_printf ("   ");
                }

                rtlglue_printf ("  %s", ascii);
        }
        rtlglue_printf ("\n");
        return;
}

