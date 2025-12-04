/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"

#define USHRT_MAX       ((u16)(~0U))
#define SHRT_MAX        ((s16)(USHRT_MAX>>1))
#define ULLONG_MAX      (~0ULL)

#define KSTRTOX_OVERFLOW        (1U << 31)
#if 0
extern _LONG_CALL_ void *
strncpy( const void *av, const void *bv, size_t len ) __attribute__((weak,
        alias("_strncpy")));
#endif

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ char *_strncpy(char *dest, const char *src, size_t count)
{
    char *tmp = dest;
    while (count) {
        if ((*tmp = *src) != 0)
            src++;
        tmp++;
        count--;
    }
    return dest;
}
#if 0
extern _LONG_CALL_ void *
strcpy(char *dest, const char *src) __attribute__((weak, alias("_strcpy")));
#endif

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ char *_strcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while ((*dest++ = *src++) != '\0')
        /* nothing */;
    return tmp;
}



INFRA_ROM_TEXT_SECTION
_LONG_CALL_ VOID
prvStrCpy(
    IN  u8  *pDES,
    IN  const u8  *pSRC
)
{

    while(1) {
        if (*pSRC != '\0') {
            *(pDES++) = *(pSRC++);

        } else {
            break;
        }
    }
    *(pDES) = '\0';

}

//4 Based on Linux string.c
#if 0
extern _LONG_CALL_ size_t
strlen(const char *s) __attribute__((weak, alias("_strlen")));
#endif

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ SIZE_T _strlen(const char *s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        ;
    return sc - s;
}

#if 0
extern _LONG_CALL_ size_t
strnlen(const char *s, size_t count) __attribute__((weak, alias("_strnlen")));
#endif

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ size_t _strnlen(const char *s, size_t count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc)
        ;

    return sc - s;
}


INFRA_ROM_TEXT_SECTION
_LONG_CALL_ u32
prvStrLen(
    IN  const   u8  *pSRC
)
{
    u32  StrLen = 0;
    while(1) {
        if (*pSRC != '\0') {
            pSRC++;
            StrLen++;

            if (StrLen == 0xFFFF) {
                break;
            }
        } else {
            break;
        }
    }

    return StrLen;
}

#if 0
extern _LONG_CALL_ int
strcmp(const char *cs, const char *ct) __attribute__((weak, alias("_strcmp")));
#endif

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ int _strcmp(const char *cs, const char *ct)
{
    unsigned char c1, c2;

    while (1) {
        c1 = *cs++;
        c2 = *ct++;

        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            break;
    }

    return 0;
}

#if 0
extern _LONG_CALL_ int
strncmp(const char *cs, const char *ct, size_t count) __attribute__((weak, alias("_strncmp")));
#endif

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ int _strncmp(const char *cs, const char *ct, size_t count)
{
    unsigned char c1, c2;

    while (count) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2)
            return c1 < c2 ? -1 : 1;
        if (!c1)
            break;
        count--;
    }

    return 0;
}



INFRA_ROM_TEXT_SECTION
_LONG_CALL_ u8
prvStrCmp(
    IN  const   u8  *string1,
    IN  const   u8  *string2
)
{
    u8  IsEqual = 0;
    u8  *pStr1 = (u8 *)string1;
    u8  *pStr2 = (u8 *)string2;

    while(1) {
        if (!(*pStr1)) {
            if(!(*pStr2)) {
                IsEqual = 0;
                break;
            } else {
                IsEqual = 1;
                break;
            }
        } else {
            if ((*pStr1) == (*pStr2)) {
                pStr1++;
                pStr2++;
            } else {
                IsEqual = 1;
                break;
            }
        }
    }


    return IsEqual;
}


INFRA_ROM_TEXT_SECTION
_LONG_CALL_ u8*
StrUpr(
    IN  u8  *string
)
{
    u8  *pStr;
    const   u8  diff = 'a' - 'A';

    pStr = string;

    while(*pStr) {
        if ((*pStr >= 'a') && (*pStr <= 'z')) {
            *pStr -= diff;
        }
        pStr++;
    }

    return string;
}

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ int prvAtoi(const char * s)
{

    int num=0,flag=0;
    int i;
    for(i=0; i<=_strlen(s); i++) {
        if(s[i] >= '0' && s[i] <= '9')
            num = num * 10 + s[i] -'0';
        else if(s[0] == '-' && i==0)
            flag =1;
        else
            break;
    }

    if(flag == 1)
        num = num * -1;

    return(num);

}

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ const char * prvStrStr(
    IN const char * str1,
    IN const char * str2
)
{
    DiagPrintf("prvStrStr: not supported yet");
}

/**
* sscanf - Unformat a buffer into a list of arguments
* @buf:        input buffer
* @fmt:        formatting of buffer
* @...:        resulting arguments
*/
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ int _sscanf(const char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = _vsscanf(buf, fmt, args);
    va_end(args);

    return i;
}

/**
 * strsep - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 *
 * strsep() updates @s to point after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ char *_strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if (sbegin == NULL)
        return NULL;

    end = _strpbrk(sbegin, ct);
    if (end)
        *end++ = '\0';
    *s = end;
    return sbegin;
}


INFRA_ROM_TEXT_SECTION
_LONG_CALL_ char *skip_spaces(const char *str)
{
    while (isspace(*str))
        ++str;
    return (char *)str;
}

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ int skip_atoi(const char **s)
{
    int i = 0;

    while (isdigit(**s))
        i = i*10 + *((*s)++) - '0';

    return i;
}


INFRA_ROM_TEXT_SECTION
_LONG_CALL_ int _vsscanf(const char *buf, const char *fmt, va_list args)
{
    const char *str = buf;
    char *next;
    char digit;
    int num = 0;
    u8 qualifier;
    unsigned int base;
    union {
        long long s;
        unsigned long long u;
    } val;
    s16 field_width;
    bool is_sign;

    while (*fmt) {
        /* skip any white space in format */
        /* white space in format matchs any amount of
            * white space, including none, in the input.
            */
        if (isspace(*fmt)) {
            fmt = skip_spaces(++fmt);
            str = skip_spaces(str);
        }

        /* anything that is not a conversion must match exactly */
        if (*fmt != '%' && *fmt) {
            if (*fmt++ != *str++) {
                break;
            }
            continue;
        }

        if (!*fmt) {
            break;
        }
        ++fmt;

        /* skip this conversion.
            * advance both strings to next white space
            */
        if (*fmt == '*') {
            if (!*str) {
                break;
            }
            while (!isspace(*fmt) && *fmt != '%' && *fmt)
                fmt++;
            while (!isspace(*str) && *str)
                str++;
            continue;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) {

            field_width = skip_atoi(&fmt);
            if (field_width <= 0) {
                break;
            }
        }

        /* get conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || _tolower(*fmt) == 'l' ||
                _tolower(*fmt) == 'z') {
            qualifier = *fmt++;

            if (unlikely(qualifier == *fmt)) {
                if (qualifier == 'h') {
                    qualifier = 'H';
                    fmt++;
                } else if (qualifier == 'l') {
                    qualifier = 'L';
                    fmt++;
                }
            }
        }

        if (!*fmt) {
            break;
        }

        if (*fmt == 'n') {
            /* return number of characters read so far */
            *va_arg(args, int *) = str - buf;
            ++fmt;
            continue;
        }

        if (!*str) {
            break;
        }

        base = 10;
        is_sign = 0;

        switch (*fmt++) {
            case 'c': {
                char *s = (char *)va_arg(args, char*);
                if (field_width == -1)
                    field_width = 1;
                do {
                    *s++ = *str++;
                } while (--field_width > 0 && *str);
                num++;
            }
            continue;
            case 's': {
                char *s = (char *)va_arg(args, char *);
                if (field_width == -1)
                    field_width = SHRT_MAX;
                /* first, skip leading white space in buffer */
                str = skip_spaces(str);

                /* now copy until next white space */
                while (*str && !isspace(*str) && field_width--) {
                    *s++ = *str++;
                }
                *s = '\0';
                num++;
            }
            continue;
            case 'o':
                base = 8;
                break;
            case 'x':
            case 'X':
                base = 16;
                break;
            case 'i':
                base = 0;
            case 'd':
                is_sign = 1;
            case 'u':
                break;
            case '%':
                /* looking for '%' in str */
                if (*str++ != '%') {
                    return num;
                }
                continue;
            default:
                /* invalid format; stop here */
                return num;
        }

        /* have some sort of integer conversion.
            * first, skip white space in buffer.
        */
        str = skip_spaces(str);

        digit = *str;
        if (is_sign && digit == '-')
            digit = *(str + 1);

        if (!digit
                || (base == 16 && !isxdigit(digit))
                || (base == 10 && !isdigit(digit))
                || (base == 8 && (!isdigit(digit) || digit > '7'))
                || (base == 0 && !isdigit(digit))) {
            break;
        }

        if (is_sign) {
            val.s = qualifier != 'L' ?
                    simple_strtol(str, &next, base) :
                    simple_strtoll(str, &next, base);
        } else {
            val.u = qualifier != 'L' ?
                    simple_strtoul(str, &next, base) :
                    simple_strtoull(str, &next, base);
        }

        if (field_width > 0 && next - str > field_width) {
            if (base == 0)
                _parse_integer_fixup_radix(str, &base);

            while (next - str > field_width) {
                if (is_sign) {
                    val.s = div_s64(val.s, base);
                } else {
                    val.u = div_u64(val.u, base);
                }
                --next;
            }
        }

        switch (qualifier) {
            case 'H':       /* that's 'hh' in format */
                if (is_sign)
                    *va_arg(args, signed char *) = val.s;
                else
                    *va_arg(args, unsigned char *) = val.u;
                break;
            case 'h':
                if (is_sign)
                    *va_arg(args, short *) = val.s;
                else
                    *va_arg(args, unsigned short *) = val.u;
                break;
            case 'l':
                if (is_sign)
                    *va_arg(args, long *) = val.s;
                else
                    *va_arg(args, unsigned long *) = val.u;
                break;
            case 'L':
                if (is_sign)
                    *va_arg(args, long long *) = val.s;
                else
                    *va_arg(args, unsigned long long *) = val.u;
                break;
            case 'Z':
            case 'z':
                *va_arg(args, size_t *) = val.u;
                break;
            default:
                if (is_sign)
                    *va_arg(args, int *) = val.s;
                else
                    *va_arg(args, unsigned int *) = val.u;
                break;
        }
        num++;

        if (!next) {
            break;
        }
        str = next;
    }

    return num;
}

/**
 * simple_strtoll - convert a string to a signed long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtoll instead.
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ long long simple_strtoll(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -simple_strtoull(cp + 1, endp, base);

    return simple_strtoull(cp, endp, base);
}


/*
 * simple_strtoull - convert a string to an unsigned long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtoull instead.
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base)
{
    unsigned long long result;
    unsigned int rv;

    cp = _parse_integer_fixup_radix(cp, &base);
    rv = _parse_integer(cp, base, &result);
    /* FIXME */
    cp += (rv & ~KSTRTOX_OVERFLOW);

    if (endp)
        *endp = (char *)cp;

    return result;
}


/**
 * simple_strtol - convert a string to a signed long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtol instead.
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ long simple_strtol(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -simple_strtoul(cp + 1, endp, base);

    return simple_strtoul(cp, endp, base);
}

/**
 * simple_strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 *
 * This function is obsolete. Please use kstrtoul instead.
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
    return simple_strtoull(cp, endp, base);
}

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ const char *_parse_integer_fixup_radix(const char *s, unsigned int *base)
{
    if (*base == 0) {
        if (s[0] == '0') {
            if (_tolower(s[1]) == 'x' && isxdigit(s[2]))
                *base = 16;
            else
                *base = 8;
        } else
            *base = 10;
    }
    if (*base == 16 && s[0] == '0' && _tolower(s[1]) == 'x')
        s += 2;
    return s;
}

INFRA_ROM_TEXT_SECTION
_LONG_CALL_ unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p)
{
    unsigned long long res;
    unsigned int rv;
    int overflow;

    res = 0;
    rv = 0;
    overflow = 0;
    while (*s) {
        unsigned int val;

        if ('0' <= *s && *s <= '9')
            val = *s - '0';
        else if ('a' <= _tolower(*s) && _tolower(*s) <= 'f')
            val = _tolower(*s) - 'a' + 10;
        else
            break;

        if (val >= base)
            break;
        /*
         * Check for overflow only if we are within range of
         * it in the max base we support (16)
         */
        if (unlikely(res & (~0ull << 60))) {
            if (res > div_u64(ULLONG_MAX - val, base))
                overflow = 1;
        }
        res = res * base + val;
        rv++;
        s++;
    }
    *p = res;
    if (overflow)
        rv |= KSTRTOX_OVERFLOW;
    return rv;
}


/**
 * div_u64 - unsigned 64bit divide with 32bit divisor
 *
 * This is the most common 64bit divide and should be used if possible,
 * as many 32bit archs can optimize this variant better than a full 64bit
 * divide.
 */
#ifndef div_u64
//static inline
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ u64 div_u64(u64 dividend, u32 divisor)
{
    u32 remainder;
    return div_u64_rem(dividend, divisor, &remainder);
}
#endif

/**
 * div_s64 - signed 64bit divide with 32bit divisor
 */
#ifndef div_s64
//static inline
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ s64 div_s64(s64 dividend, s32 divisor)
{
    s32 remainder;
    return div_s64_rem(dividend, divisor, &remainder);
}
#endif

/**
 * div_u64_rem - unsigned 64bit divide with 32bit divisor with remainder
 *
 * This is commonly provided by 32bit archs to provide an optimized 64bit
 * divide.
 */
//static inline
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ u64 div_u64_rem(u64 dividend, u32 divisor, u32 *remainder)
{
    *remainder = (u32)dividend % divisor;
    return (u32)dividend / divisor;
}

/**
 * div_s64_rem - signed 64bit divide with 32bit divisor with remainder
 */
//static inline
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ s64 div_s64_rem(s64 dividend, s32 divisor, s32 *remainder)
{
    *remainder = (s32)dividend % divisor;
    return (s32)dividend / divisor;
}



/**
 * strpbrk - Find the first occurrence of a set of characters
 * @cs: The string to be searched
 * @ct: The characters to search for
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ char *_strpbrk(const char *cs, const char *ct)
{
    const char *sc1, *sc2;

    for (sc1 = cs; *sc1 != '\0'; ++sc1) {
        for (sc2 = ct; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return NULL;
}


/**
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
INFRA_ROM_TEXT_SECTION
_LONG_CALL_ char *_strchr(const char *s, int c)
{
    for (; *s != (char)c; ++s)
        if (*s == '\0')
            return NULL;
    return (char *)s;
}

