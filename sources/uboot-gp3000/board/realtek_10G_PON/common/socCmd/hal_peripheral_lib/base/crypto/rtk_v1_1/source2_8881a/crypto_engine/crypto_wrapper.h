#ifndef __CRYPTO_WRAPPER_H
#define __CRYPTO_WRAPPER_H

#ifdef __mips__

#define printf prom_printf
#define printk prom_printf
#define rtlglue_printf prom_printf
#define sprintf crypto_sprintf
#define memDump crypto_memDump

int crypto_sprintf(char *buf, const char *fmt, ...);
void crypto_memDump(void *start, uint32 size, int8 * strHeader);

#endif

#endif
