#include <config.h>
#include <common.h>
#include <asm/arch/bspchip.h>




/******************************************************************************
*
* serial_init - initialize a channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/
#ifdef CONFIG_UART1
#define DISABLED_UART	0xC000
#define ENABLED_UART	0x0300
#else
#define DISABLED_UART	0x0300
#define ENABLED_UART	0xC000
#endif

#if 0 /* Use the one in otto_uart.c */
int serial_init (void)
{
	uint val;
#ifdef CONFIG_RTL8686
	/* Enable UART0, UART1 and UART DECT */
	*((volatile unsigned int *)0xbb023020) = *((volatile unsigned int *)0xbb023020) | (7<<3);
#endif
	/* */	

	val = REG32(BSP_MISC_PINMUX) & (~DISABLED_UART);
	REG32(BSP_MISC_PINMUX) = (val | ENABLED_UART);

	serial_setbrg();
	
	return 0;
}

void serial_setbrg (void)
{
	unsigned short divisor;
	int i ;
	REG8(_UART_LCR) = 0x83;
	
	divisor = (board_LX_freq_hz() / ( 16 * CONFIG_BAUDRATE )) - 1;
   	REG8(_UART_DLM) = (divisor >> 8) & 0x00FF;
    	REG8(_UART_DLL) = divisor & 0x00FF;
	
	REG8(_UART_LCR) = 0x03;

	REG8(_UART_FCR) = 0x06;
	for(i=0;i<1000;i++);
	REG8(_UART_FCR) = 0x01;
    
    REG8(_UART_IER) = 0x00;
    REG8(_UART_MCR) = 0x03;	
}

void serial_putc (const char c)
{
	if (c == '\n') serial_putc ('\r');
	
	while (0 == (REG8(_UART_LSR) & 0x20))
		/* nop */ ;
	
	REG8(_UART_THR) = c;
}

void serial_puts (const char *s)
{
    while (*s)
    {
	serial_putc (*s++);
    }
}

int serial_tstc (void) {
	if (1 == (REG8(_UART_LSR) & 0x01))
		return 1;
	else
		return 0;
}

int serial_getc (void)
{
	while (!serial_tstc())
		;
	
    return REG8(_UART_RBR);    
}
#endif

