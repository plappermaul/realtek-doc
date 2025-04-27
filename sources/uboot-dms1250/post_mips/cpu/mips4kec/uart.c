/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

/*
 * UART test
 *
 * The Serial Management Controllers (SMC) and the Serial Communication
 * Controllers (SCC) listed in ctlr_list array below are tested in
 * the loopback UART mode.
 * The controllers are configured accordingly and several characters
 * are transmitted. The configurable test parameters are:
 *   MIN_PACKET_LENGTH - minimum size of packet to transmit
 *   MAX_PACKET_LENGTH - maximum size of packet to transmit
 *   TEST_NUM - number of tests
 */

#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_UART
#include <command.h>
#include <serial.h>
#include "rtk_switch.h"

static const int bauds[] = { 9600, 115200 };

#define UART_BASE_ADDR (0xb8002000)
#define UART_ADDR(channel,offset) (UART_BASE_ADDR+(0x100*channel)+offset)

static int uart_post_get(int chan,unsigned char *c)
{
	int i;
	int result;
	
	result=1;
	*c=0;
	for (i=0;i<1000;i++)
	{
		if (serial_chan_tstc(chan))
		{
			*c= serial_chan_getc(chan);
			result = 0;
			break;
		}
		else
			udelay(3000);
	}
	//putc(*c);
	return result;
	
}
int uart_post_test(int flags)
{
	unsigned int lsr1[CONSOLE_NUM_PORTS],lsr2[CONSOLE_NUM_PORTS],lsr3[CONSOLE_NUM_PORTS];
	unsigned char c,ret_c[CONSOLE_NUM_PORTS];
	int ret_chan[CONSOLE_NUM_PORTS],baud_chan[CONSOLE_NUM_PORTS];
	int chan;
	int ret, b;

	/* Save current serial state */
	ret = 0;

	for (chan = 0; chan < CONSOLE_NUM_PORTS; chan++) {
		ret_chan[chan] = 0;
		ret_c[chan]=0x0;
		ret = serial_init(chan,CONFIG_BAUDRATE);
		if (ret)
			goto done;
			
		/* Consume anything that happens to be queued */
		while (serial_chan_tstc(chan))
			serial_chan_getc(chan);
	
		serial_IoCtl(chan,SERIAL_IO_BAUD,CONFIG_BAUDRATE);

		/* Test every available baud rate */
		for (b = 0; b < ARRAY_SIZE(bauds); ++b) {
			serial_IoCtl(chan,SERIAL_IO_BAUD,bauds[b]);

			/* Enable loop back */
			serial_IoCtl(chan,SERIAL_IO_LOOPBACK,1);
			
			while (serial_chan_tstc(chan))
					serial_chan_getc(chan);
			/*
			 * Stick to printable chars to avoid issues:
			 *  - terminal corruption
			 *  - serial program reacting to sequences and sending
			 *    back random extra data
			 *  - most serial drivers add in extra chars (like \r\n)
			 */
			for (c = 0x41; c < 0x7B; c++) {
				while (serial_chan_tstc(chan))
					serial_chan_getc(chan);
				/* Send it out */
				serial_chan_putc(chan,c);
				
				lsr1[chan] = *((volatile u32_t *)(UART_ADDR(chan,0x14)));

				ret=uart_post_get(chan,&ret_c[chan]);
				/* Make sure it's the same one */
				if (ret | (c != ret_c[chan])) {
					lsr2[chan] = *((volatile u32_t *)(UART_ADDR(chan,0x14)));
					baud_chan[chan]= bauds[b];
					ret_c[chan] = *((volatile unsigned char *)UART_ADDR(chan,0));
					ret_chan[chan] |= (0x10000000 | ((int)c));
					goto done;
				}
				else
				{
					serial_chan_putc(chan,'\b');
					udelay(30000);
				}
			}
		}
		/* Disable loop back */
		lsr3[chan] = *((volatile u32_t *)(UART_ADDR(chan,0x14)));
		serial_IoCtl(chan,SERIAL_IO_LOOPBACK,0);
	}

 done:

/* Disable loop back */
	for (chan = 0; chan < CONSOLE_NUM_PORTS; chan++) {
		serial_IoCtl(chan,SERIAL_IO_LOOPBACK,0);
		serial_IoCtl(chan,SERIAL_IO_BAUD,CONFIG_BAUDRATE);
		if (ret_chan[chan])
		{
			ret |= ret_chan[chan];
			post_log ("uart %d test failed %x ret_c = %x,baud = %d \n",chan,ret_chan[chan],ret_c[chan],baud_chan[chan]);
		}
	}

	return ret;
}
#endif
