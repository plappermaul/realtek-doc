#include <common.h>
#include <soc.h>
#include <pblr.h>
#include <asm/otto_pll.h>
#include "rtk_switch.h"

DECLARE_GLOBAL_DATA_PTR;

static int calc_divisor (int baudrate) {
#define MODE_X_DIV 16
	return ((CONFIG_SYS_HZ + (baudrate * (MODE_X_DIV/2))) /
	        (MODE_X_DIV * baudrate)) + (CONFIG_UART_DIVISOR_MOD);
}

int serial_init (int chan,int baudrate) {
	
	if (baudrate == 0x0) {
	} else {
		serial_setbrg(chan,baudrate);
	}
	return (0);
}

void serial_putc(const char c) {
	if (c == '\n')
		parameters._uart_putc(CONSOLE_CH_INDEX,'\r');

	parameters._uart_putc(CONSOLE_CH_INDEX,c);
	return;
}

void serial_putc_raw(const char c) {
	parameters._uart_putc(CONSOLE_CH_INDEX,c);
	return;
}

void serial_puts(const char *s) {
	while (*s) {
		serial_putc(*s++);
	}
	return;
}

void serial_chan_putc(int chan,const char c) {
	parameters._uart_putc(chan,c);
	return;
}

void serial_chan_puts(int chan,const char *s) {
	char c;
	while (*s) {
		c = *s++;
		//serial_poe_putc(chan,*s++);
		parameters._uart_putc(chan,c);
	}
	return;
}

int serial_chan_getc(int chan) {
	return parameters._uart_getc(chan);
}

int serial_chan_tstc(int chan) {
	return parameters._uart_tstc(chan);
}

int serial_getc() {
	return parameters._uart_getc(CONSOLE_CH_INDEX);
}

int serial_tstc() {
	return parameters._uart_tstc(CONSOLE_CH_INDEX);
}

void serial_setbrg(int chan,int baudrate) {
	parameters._uart_init(chan,calc_divisor(baudrate));
	return;
}

int serial_IoCtl(int chan,int request,int arg)
{
	if (chan >= CONSOLE_NUM_PORTS)
	{
		return FAILED;
	}
	switch(request)
	{
		case SERIAL_IO_BAUD:
			arg=calc_divisor(arg);
			break;
		case SERIAL_IO_LOOPBACK:
			break;
		default:
			return FAILED;
	}
	
	return parameters._uart_ctrl(chan,request,arg);
}
