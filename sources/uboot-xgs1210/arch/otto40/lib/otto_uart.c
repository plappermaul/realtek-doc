#include <common.h>
#include <soc.h>
#include <asm/otto_pll.h>

DECLARE_GLOBAL_DATA_PTR;

//#define parameters __BIOS
//#define _uart_putc uart_putc
//#define _uart_getc uart_getc
//#define _uart_tstc uart_tstc

#define UART_BASE_ADDR (0xb8002000)
#define UART_OFFSET    (0x100)
#define DEF_UART       (0)
#define UART_REG(chan, offset) REG32(UART_BASE_ADDR+chan*UART_OFFSET+offset)

int serial_init (int chan, int baudrate) {
	if (gd->baudrate == 0x0) {
	} else {
		serial_setbrg(chan, baudrate);
	}
	return (0);
}
void __serial_putc(int chan, const char c) {
	while ((UART_REG(chan, 0x14) & 0x20000000) == 0);
	UART_REG(chan, 0x0) = c<<24;
	return;
}

int __serial_getc(int chan) {
	while ((UART_REG(chan, 0x14) & 0x8F000000) != 0x01000000);
	return  UART_REG(chan, 0x0) >>24;
}

int __serial_tstc(int chan) {
	return (UART_REG(chan, 0x14) & 0x8F000000) == 0x01000000;
}

void serial_putc(const char c) {
	if (c == '\n')
        	__serial_putc(DEF_UART, '\r');

	__serial_putc(DEF_UART, c);
	return;
}

void serial_putc_raw(const char c) {
	__serial_putc(DEF_UART, c);
	return;
}

void serial_puts(const char *s) {
	while (*s) {
		serial_putc(*s++);
	}
	return;
}

int serial_getc(void) {
	return __serial_getc(DEF_UART);
}

int serial_tstc(void) {
	return __serial_tstc(DEF_UART);
}

void serial_setbrg(int chan,int baudrate) {
	u32_t baud_divisor;
#define MHZ_TO_HZ(mhz) ((mhz)*1000000)
	/* Foumula: baudrate = LX_CLK_HZ/(16*divisor)
        	  -->divisor = LX_CLK_HZ/(16*baudrate) */
	u32_t f=otto_cg_query_freq(CG_DEV_LX);
        if (0==f) { f=200; printf("WW: otto_cg_query_freq failed!\n"); }
	baud_divisor = MHZ_TO_HZ(f) / (16 * baudrate)-1;
//        REG32(0xbf007000) = otto_cg_query_freq(CG_DEV_LX);
//        REG32(0xbf007004) = UART_BASE_ADDR+chan*UART_OFFSET+0x8;
//        REG32(0xbf007008) = baud_divisor;
#define LCR_BKSE 0x80                /* Bank select enable */
#define LCRVAL   LCR_8N1             /* 8 data, 1 stop, no parity */
#define MCRVAL   (MCR_DTR | MCR_RTS) /* RTS/DTR */
#define FCRVAL   0xC1                /* Clear & enable FIFOs */
#define LCR_8N1  0x03

#define MCR_DTR  0x01
#define MCR_RTS  0x02
	UART_REG(chan, 0x4)  = 0;
	UART_REG(chan, 0xC)  = ((LCR_BKSE | LCRVAL) << 24);
	UART_REG(chan, 0x0)  = (baud_divisor)      << 24;
	UART_REG(chan, 0x4)  = (baud_divisor >> 8) << 24;
	UART_REG(chan, 0xC)  = ((LCRVAL) << 24);
	UART_REG(chan, 0x10) = ((MCRVAL) << 24);
	UART_REG(chan, 0x8)  = ((FCRVAL) << 24);
	UART_REG(chan, 0x10) = ((MCRVAL) << 24) | (0x40000000);

	return;
}
