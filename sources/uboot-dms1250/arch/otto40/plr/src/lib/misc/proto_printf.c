// ---------------------------------------------------------------
//  The new printf implement uses much less SRAM and stack area.
// ---------------------------------------------------------------

#include <soc.h>
#include <stdarg.h>
#include <proto_printf.h>

#ifndef PRINTF_SECTION
    #define PRINTF_SECTION
#endif

#define ZEROPAD 1  /* pad with zero */
#define SIGN    2  /* unsigned/signed long */
#define PLUS    4  /* show plus */
#define SPACE   8  /* space if plus */
#define LEFT    16 /* left justified */
#define SPECIAL 32 /* 0x */
#define LARGE   64 /* use 'ABCDEF' instead of 'abcdef' */

#define is_digit(c) ((c) >= '0' && (c) <= '9')

#define do_div(n, base) ({\
			int __res = ((unsigned long) n) % (unsigned) base; \
			n = ((unsigned long) n) / (unsigned) base; \
			__res; \
		})


#define _proto_strnlen(s, c) ({\
            const char *sc=s;\
            int count=c; \
            for (; count-- && *sc != '\0'; ++sc);\
            (int)(sc - s);\
        })
#if 0
static inline int 
__proto_strnlen(const char * s, int count) {
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc);
	return sc - s;
}
#endif

#define skip_atoi(cs) ({\
            const char **s=cs;\
            int i=0;\
            while (is_digit(**s)) i = i*10 + *((*s)++) - '0';\
            i;\
        })
#if 0
static inline int 
_skip_atoi(const char **s) {
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}
#endif

#if 0
// this is used for old implementation which utilizes the table lookup
const static char *__lower_digits="0123456789abcdefghijklmnopqrstuvwxyz";
const static char *__upper_digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#endif

#define my_putc(chr) ({(*proto_putc)(chr, user); ++cc;})
#define my_putcn(chr) ({char __chr=chr; if (__chr=='\n') (*proto_putc)('\r', user); (*proto_putc)(__chr, user); ++cc;})



PRINTF_SECTION  static unsigned int
_proto_number(proto_putc_t *proto_putc, void *user, long num, int base, int size, int precision ,int type) {
    unsigned int cc=0;
    int i;
	char c,sign,tmp[66];
	
	//const char *digits=__lower_digits;
	char char_a_10 = ((type & LARGE) ? 'A' : 'a')-10;
	#define get_digit(n) ({const unsigned char c=n; (c<10)?c+'0':c+char_a_10;})
	
	

	// if (type & LARGE) digits = __upper_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0) {
		tmp[i++]='0';
	} else {
	    while (num != 0) {
	        int j=do_div(num, base);
		    tmp[i++] = get_digit(j); //digits[j];
		}
    }
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0) {
			//*str++ = ' ';
			my_putc(' ');
		}
	if (sign) {
		//*str++ = sign;
		my_putc(sign);
	}
	if (type & SPECIAL) {
		if (base==8) {
			//*str++ = '0';
			my_putc('0');
		} else if (base==16) {
			//*str++ = '0';
			my_putc('0');
			
			const char char_x=get_digit(33); //digits[33];
			//*str++ = char_x;
			my_putc(char_x);
		}
	}
	if (!(type & LEFT))
		while (size-- > 0) {
			//*str++ = c;
			my_putc(c);
		}
	while (i < precision--) {
		//*str++ = '0';
		my_putc('0');
	}
	while (i-- > 0) {
		//*str++ = tmp[i];
		my_putc(tmp[i]);
	}
	while (size-- > 0) {
		//*str++ = ' ';
		my_putc(' ');
	}
	//return str;
	return cc;
}


PRINTF_SECTION unsigned int
proto_printf(proto_putc_t *proto_putc, void *user, const char *fmt, ...){
    if (proto_putc==VZERO) return 0;
    unsigned int cc=0;
	int len;
	unsigned long num;
	int i, base;
	const char *s;
	int flags;		/* flags to number() */
	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'q' for integer fields */
	va_list args;
	
	va_start(args, fmt);
	
	
	

	//for (str=buf ; *fmt ; ++fmt) {
	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			//*str++ = *fmt;
			my_putcn(*fmt);
			continue;
		}

		/* process flags */
		flags = 0;
	repeat:
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
		case '-': flags |= LEFT; goto repeat;
		case '+': flags |= PLUS; goto repeat;
		case ' ': flags |= SPACE; goto repeat;
		case '#': flags |= SPECIAL; goto repeat;
		case '0': flags |= ZEROPAD; goto repeat;
		}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'q') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT)) {
				while (--field_width > 0) {
					//*str++ = ' ';
					my_putc(' ');
				}
            }
			//*str++ = (unsigned char) va_arg(args, int);
			{   
			    char temp_chr=(unsigned char) va_arg(args, int);
			    my_putcn(temp_chr);
			}
			while (--field_width > 0) {
				//*str++ = ' ';
				my_putc(' ');
			}
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = _proto_strnlen(s, precision);

			if (!(flags & LEFT)) {
				while (len < field_width--) {
					//*str++ = ' ';
					my_putc(' ');
				}
            }
			for (i = 0; i < len; ++i) {
				//*str++ = *s++;
				my_putcn(*s++);
			}
			while (len < field_width--) {
				//*str++ = ' ';
				my_putc(' ');
			}
			continue;

		case 'p':
			if (field_width == -1) {
				/* 121025,JJ,the final '+2' is for the additional '0x'. */
				field_width = 2*sizeof(void *) + 2;
				flags |= ZEROPAD;
			}
			flags |= SPECIAL;
			/*
			str = _proto_number(str,
			             (unsigned long) va_arg(args, void *), 16,
			             field_width, precision, flags);
            */
            cc += _proto_number(proto_putc, user, (unsigned long) va_arg(args, void *), 16,
                field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				//*ip = (str - buf);
				*ip=cc;
			} else {
				int * ip = va_arg(args, int *);
				//*ip = (str - buf);
				*ip=cc;
			}
			continue;

		case '%':
			//*str++ = '%';
			my_putc('%');
			continue;

			/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			//*str++ = '%';
			my_putc('%');
			if (*fmt) {
				//*str++ = *fmt;
				my_putcn(*fmt);
			} else {
				--fmt;
			}
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		//str = _proto_number(str, num, base, field_width, precision, flags);
		cc += _proto_number(proto_putc, user, num, base, field_width, precision, flags);
	}
	va_end (args);
	//*str = '\0';
	//return str-buf;
	return cc;
}


