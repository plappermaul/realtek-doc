/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "hal_all.h"
#include "basic_types.h"
#include <stdarg.h>
#include "hal_api.h"
#include "section_config.h"

#ifdef CONFIG_DEBUG_LOG
u32 ConfigDebugErr=0xffffffff;
u32 ConfigDebugInfo=_DBG_CRYPTO_; /* jwsyu 20150331 add crypto debug info */
u32 ConfigDebugTrace=0;
u32 ConfigDebugWarn=0;
#endif

#if 0
u32
VSprintf(
    INOUT       u8 *buf, 
    INOUT const u8 *fmt, 
    INOUT const u32 *dp
)
{
	u8 *p, *s;
	s = buf;
	for ( ; *fmt != '\0'; ++fmt) {
		if (*fmt != '%') {
			buf ? *s++ = *fmt : PutChar(*fmt);
			continue;
		}
		if (*++fmt == 's') {
			for (p = (char *)*dp++; *p != '\0'; p++)
				buf ? *s++ = *p : PutChar(*p);
		}
		else {	/* Length of item is bounded */
			u8  tmp[20], *q = tmp;
			u32 alt = 0;
			u32 shift = 28;

			if ((*fmt  >= '0') && (*fmt  <= '9'))
			{
				u32 width;
				u8  fch = *fmt;
		                for (width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
		                {    width = width * 10 + fch - '0';
		                }
				  shift=(width-1)*4;
			}

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */
			if ((*fmt == 'x')||(*fmt == 'X')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const u64 *lp = (const u64 *)dp;
				u64 h = *lp++;
				u32 ncase = (*fmt & 0x20);
				dp = (const u32 *)lp;
				if (alt) {
					*q++ = '0';
					*q++ = 'X' | ncase;
				}
				for ( ; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if (*fmt == 'd') {
				u32 i = *dp++;
				u8 *r;
				if (i < 0) {
					*q++ = '-';
					i = -i;
				}
				p = q;		/* save beginning of digits */
				do {
					*q++ = '0' + (i % 10);
					i /= 10;
				} while (i);
				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */
				while (--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}		
			else if (*fmt == 'c')
				*q++ = *dp++;
			else
				*q++ = *fmt;
			/* now output the saved string */
			for (p = tmp; p < q; ++p)
				buf ? *s++ = *p : PutChar(*p);
		}
	}
	if (buf)
		*s = '\0';
    
	return (s - buf);

}
#else
INFRA_ROM_TEXT_SECTION
int VSprintf(char *buf, const char *fmt, const int *dp)
{
	char *p, *s;

	s = buf;
	
	
	for ( ; *fmt != '\0'; ++fmt) {
		if (*fmt != '%') {
			buf ? *s++ = *fmt : DiagPutChar(*fmt);
			continue;
		}
		if (*++fmt == 's') {
			for (p = (char *)*dp++; *p != '\0'; p++)
				buf ? *s++ = *p : DiagPutChar(*p);
		}
		else {	/* Length of item is bounded */
			char tmp[20], *q = tmp;
			int alt = 0;
			int shift = 28;

#if 1   //wei patch for %02x
			if ((*fmt  >= '0') && (*fmt  <= '9'))
			{
				int width;
				unsigned char fch = *fmt;
		                for (width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
		                {    width = width * 10 + fch - '0';
		                }
				  shift=(width-1)*4;
			}
#endif

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */

			if ( (*fmt == 'p') || (*fmt == 'P')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				long h = *lp++;
				int ncase = (*fmt & 0x20);
				dp = (const int *)lp;
				if((*fmt == 'p') || (*fmt == 'P'))
					alt=1;
				if (alt) {
					*q++ = '0';
					*q++ = 'X' | ncase;
				}
				for ( ; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if ((*fmt == 'x')||(*fmt == 'X')) {
				/* fix for armv8 64bit */
				//const long *lp = (const long *)dp;
				int h = *dp++;
				int ncase = (*fmt & 0x20);
				//dp = (const int *)lp;
				for ( ; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if (*fmt == 'd') {
				int i = *dp++;
				char *r;
				if (i < 0) {
					*q++ = '-';
					i = -i;
				}
				p = q;		/* save beginning of digits */
				do {
					*q++ = '0' + (i % 10);
					i /= 10;
				} while (i);
				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */
				while (--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}
#if 0	
			else if (*fmt == '@') {
				unsigned char *r;
				union {
					long		l;
					unsigned char	c[4];
				} u;
				const long *lp = (const long *)dp;
				u.l = *lp++;
				dp = (const int *)lp;
				for (r = &u.c[0]; r < &u.c[4]; ++r)
					q += SprintF(q, "%d.", *r);
				--q;
			}
#endif
#if 0
			else if (*fmt == '!') {
				char *r;
				p = (char *)*dp++;
				for (r = p + ETH_ALEN; p < r; ++p)
					q += SprintF(q, "%hhX:", *p);
				--q;
			}
#endif			
			else if (*fmt == 'c')
				*q++ = *dp++;
			else
				*q++ = *fmt;
			/* now output the saved string */
			for (p = tmp; p < q; ++p)
				buf ? *s++ = *p : DiagPutChar(*p);
		}
	}
	if (buf)
		*s = '\0';
	return (s - buf);
}

#endif

INFRA_ROM_TEXT_SECTION
u32 
DiagPrintf(
    IN  char *fmt, ...
)
{
	(void)VSprintf(0, fmt, ((const int *)&fmt)+1);	
    return _TRUE;
}
