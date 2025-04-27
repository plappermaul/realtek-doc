#ifndef STRING_H
#define STRING_H

#define inline_ascii_value(c, base) ({char __ascii_c=c; int __ascii_base=base; int __ascii_r;\
    if ((__ascii_c<='9') && (__ascii_c>='0')) {__ascii_r=__ascii_c-'0';} \
    else {__ascii_c|=32;__ascii_r=((__ascii_base==16)&&(__ascii_c<='f')&&(__ascii_c>='a'))?(__ascii_c-'a'+10):-1;}\
    __ascii_r;})
#define digit_char(dig) ({unsigned __dig=dig; (__dig+((__dig>=10)?('a'-10):'0'));})
unsigned int atoi(const char *v);
unsigned int ltostr(char *buf, unsigned int v, int base, int ndigit);
char *strcpy(char *dst, const char *src);
unsigned int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *memcpy(char *dst, const char *src, unsigned int len);
char *mass_copy(char *dst, const char *src, unsigned int len);
char *memset(char *dst, char value, unsigned int len);
#endif




