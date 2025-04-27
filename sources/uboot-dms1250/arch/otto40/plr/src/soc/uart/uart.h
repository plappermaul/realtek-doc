#ifndef UART_H
#define UART_H

int uart_getc(void);
void uart_putc(const char);
void uart_putcn(const char);
int uart_tstc(void);
UTIL_FAR int uart_init(unsigned int brate, unsigned int bus_mhz);


#endif



