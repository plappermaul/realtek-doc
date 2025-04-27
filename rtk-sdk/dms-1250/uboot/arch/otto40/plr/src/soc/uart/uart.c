#include <soc.h>

#define LCR_BKSE 0x80                /* Bank select enable */
#define LCRVAL   LCR_8N1             /* 8 data, 1 stop, no parity */
#define MCRVAL   (MCR_DTR | MCR_RTS) /* RTS/DTR */
#define FCRVAL   0xC1                /* Clear & enable FIFOs */
#define LCR_8N1  0x03

#define MCR_DTR  0x01
#define MCR_RTS  0x02

#ifndef SECTION_UART
    #define SECTION_UART
#endif

#ifndef SECTION_RECYCLE
    #define SECTION_RECYCLE
#endif

SECTION_UART int 
uart_getc(void) {
    while ((REG32(UART_BASE_ADDR+0x14) & 0x8F000000) != 0x01000000);
    return  REG32(UART_BASE_ADDR)>>24;
}


SECTION_UART void 
uart_putc(int c) {
    while ((REG32(UART_BASE_ADDR+0x14) & 0x20000000) == 0);
    REG32(UART_BASE_ADDR) = c<<24;
}

SECTION_UART void
uart_putcn(int ch) {
    if (ch=='\n') uart_putc('\r'); 
    uart_putc(ch);
}


SECTION_UART int 
uart_tstc(void) {
    return (REG32(UART_BASE_ADDR+0x14) & 0x8F000000) == 0x01000000;
}


UTIL_FAR SECTION_UART int
uart_init(u32_t brate, u32_t bus_mhz) {
    u32_t baud_divisor;
#define MHZ_TO_HZ(mhz) ((mhz)*1000000)

    /* Foumula: baudrate = LX_CLK_HZ/(16*divisor)
             -->divisor = LX_CLK_HZ/(16*baudrate) */
    baud_divisor = MHZ_TO_HZ(bus_mhz) / (16 * brate)-1;

    REG32(UART_BASE_ADDR+0x4)   = 0;
    REG32(UART_BASE_ADDR+0xc)   = ((LCR_BKSE | LCRVAL) << 24);
    REG32(UART_BASE_ADDR)       = (baud_divisor)      << 24;
    REG32(UART_BASE_ADDR+0x4)   = (baud_divisor >> 8) << 24;
    REG32(UART_BASE_ADDR+0xc)   = ((LCRVAL) << 24);
    REG32(UART_BASE_ADDR+0x10)  = ((MCRVAL) << 24);
    REG32(UART_BASE_ADDR+0x8)   = ((FCRVAL) << 24);
    REG32(UART_BASE_ADDR+0x10)  = ((MCRVAL) << 24) | (0x40000000);

    return 0;
}

