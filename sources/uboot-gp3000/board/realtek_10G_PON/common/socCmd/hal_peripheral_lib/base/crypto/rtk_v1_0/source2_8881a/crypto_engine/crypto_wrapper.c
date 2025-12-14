#include "crypto_types.h"
#include "crypto_wrapper.h"

int crypto_sprintf(char *buf, const char *fmt, ...)
{
	return vsprintf(buf, fmt, ((const int *)&fmt)+1);
}

void crypto_memDump(void *start, uint32 size, int8 * strHeader)
{
	int32 row, column, index, index2, max;
	uint32 buffer[5];
	uint8 *buf, *line, ascii[17];
	int8 empty = ' ';

	if(!start ||(size==0))
		return;

	line = (uint8*)start;

	/*
	16 bytes per line
	*/
	if (strHeader)
		prom_printf ("%s", strHeader);

	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) 
	{
		buf = (uint8*)line;
		memset (ascii, 0, 17);

		max = (index == row - 1) ? column : 16;
		if ( max==0 ) break; /* If we need not dump this line, break it. */

		prom_printf ("\n%08x ", (memaddr) line);
		
		//Hex
		for (index2 = 0; index2 < max; index2++)
		{
			if (index2 == 8)
			prom_printf ("  ");
			prom_printf ("%02x ", (uint8) buf[index2]);
			ascii[index2] = ((uint8) buf[index2] < 32) ? empty : buf[index2];
		}

		if (max != 16)
		{
			if (max < 8)
				prom_printf ("  ");
			for (index2 = 16 - max; index2 > 0; index2--)
				prom_printf ("   ");
		}

		//ASCII
		prom_printf ("  %s", ascii);
	}

	prom_printf ("\n");
	return;
}

