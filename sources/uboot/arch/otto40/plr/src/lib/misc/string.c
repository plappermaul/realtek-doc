#include <soc.h>
#include <lib/misc/string.h>

#ifndef SECTION_STRING
    #define SECTION_STRING
#endif

SECTION_STRING unsigned int
atoi(const char *v) {
    int base=10, d, r=0;
    if (*v=='\0') return 0;
    if ((v[0]=='0') && ((v[1]&~32)=='X')) {
        v+=2;
        base=16;
    }
    while ((d=inline_ascii_value(*(v++), base))>=0) {
        r *= base;
        r += d;
    }
    return r;
}

#if 0   // this function may be replaced by sprintf
SECTION_STRING static char *
_ltostr(char *buf, u32_t v, int base, int ndigit) {
    u32_t q, r;
    q=v/base;
    r=v%base;
    if ((v>=base)||(ndigit>0)) {
        buf=_ltostr(buf, q, base, ndigit-1);
    }
    *(buf++)=digit_char(r);
    return buf;
}

SECTION_STRING unsigned int
ltostr(char *dst_buf, unsigned int v, int base, int ndigit) {
    char *buf_start=dst_buf;
    if (base<0) {
        base=-base;
        if (v&0x80000000) {
            *(dst_buf++)='-';
            v=-v;
        }
    }
    if (base==16) {
        *(dst_buf++)='0';
        *(dst_buf++)='x';
    } else if (base==8) {
        *(dst_buf++)='0';
    }
    dst_buf=_ltostr(dst_buf, v, base, ndigit-1);
    *dst_buf='\0';
    return dst_buf - buf_start;
}
#endif
SECTION_STRING char *
strcpy(char *dst, const char *src) {
    inline_strcpy(dst, src); 
    return dst;
}
SECTION_STRING unsigned int
strlen(const char *s) {
    unsigned int r=0;
    while (*(s++)!='\0') ++r;
    return r;
}
SECTION_STRING int
strcmp(const char *s1, const char *s2) {
    return inline_strcmp(s1, s2);
}
SECTION_STRING char *
memcpy(char *dst, const char *src, unsigned int len) {
    inline_memcpy(dst, src, len);
    return dst;
}

SECTION_STRING char *
mass_copy(char *dst, const char *src, unsigned int len) {
    // len should be a multiple of 32 byte (a cache line size)
    inline_mass_copy(dst, src, len);
    return dst;
}

SECTION_STRING char *
memset(char *dst, char value, unsigned int len) {
    inline_memset(dst, value, len);
    return dst;
}
