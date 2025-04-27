#ifndef BIOS_IO_H
#define BIOS_IO_H

#define _bios (_soc.bios)
#define _soc_header (_soc.bios.header)

#define _putc(chr)  ({(*_bios.uart_putc)(chr);})
#define putc(chr)   ({if (_bios.uart_putc) _putc(chr);})
#define _putcn(chr) ({char ch=chr; if (ch=='\n') _putc('\r'); _putc(ch);})
#define putcn(chr)  ({if (_bios.uart_putc) _putcn(chr);})
#define _inline_puts(s) ({const char *p=s; char c; while ((c=*(p++))!='\0') _putcn(c);})
#define inline_puts(s)  ({if (_bios.uart_putc) _inline_puts(s);})
#define _getc()     ({(*_bios.uart_getc)();})
#define getc()      ((_bios.uart_getc)?_getc():(-1))
#define _tstc()     ({(*_bios.uart_tstc)();})
#define tstc()      ((_bios.uart_tstc)?_tstc():(0))

    
#define dcache_wr_inv_all() ({(*_bios.dcache_writeback_invalidate_all)();})
#define dcache_wr_inv(from, to) ({u32_t __from=(from), __to=(to); (*_bios.dcache_writeback_invalidate_range)(__from, __to);})
#define icache_inv_all() ({(*_bios.icache_invalidate_all)();})
#define icache_inv(from, to) ({u32_t __from=(from), __to=(to); (*_bios.icache_invalidate_range)(__from, __to);})

#endif //BIOS_IO_H

