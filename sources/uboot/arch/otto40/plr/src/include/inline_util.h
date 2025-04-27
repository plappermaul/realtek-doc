#ifndef INLINE_UTIL_H
#define INLINE_UTIL_H


#define __CAT2(prefix, l) prefix ## l
#define CAT2(prefix, l) __CAT2(prefix, l)
#define CL(prefix) CAT2(prefix, __LINE__) // cat line, to prevent redefinition

#define _TO_STR(_s) #_s
#define TO_STR(s) _TO_STR(s)

#define inline_bzero(p, n) ({char *q=(char*)p; unsigned m=n; while (m-- >0) *(q++)=0;})
#define inline_wzero(p, n) ({unsigned *q=(unsigned*)p; unsigned m=n/sizeof(unsigned); while (m-- >0) *(q++)=0;})
#define inline_strcpy(d, s) ({char *__d=d, __t; const char *__s=s; do __t=*(__d++)=*(__s++); while (__t!=0); })
#define inline_strcmp(a, b) ({const char *__a=a, *__b=b; while (*__a==*__b) {++__a;++__b;} (*__a - *__b);})
#define inline_memset(d, v, l) ({char *__d=(char*)d; char __v=(char)v; unsigned __l=l; while (__l-- >0) *(__d++)=__v;})
#define inline_wmemset(d, v, l) ({unsigned *__d=(unsigned*)d; unsigned __v=(unsigned)v; unsigned __l=l; while (__l>0) {*(__d++)=__v; __l-=sizeof(unsigned);}})
#define inline_memcpy(d, s, l) ({char *__d=(char*)d;const char *__s=(const char *)s;unsigned __l=l;while (__l-- >0) *(__d++)=*(__s++);})  
#define inline_wmemcpy(d, s, l) ({unsigned *__d=(unsigned*)d;const unsigned *__s=(const unsigned *)s;unsigned __l=l/4;while (__l-- >0) *(__d++)=*(__s++);})  
#define inline_backward_memcpy(d, s, l) ({unsigned __l=l;char *__d=(char*)(d+__l);const char *__s=(const char *)(s+__l);while (__l-- >0) *(--__d)=*(--__s);})  

// l should be a multiple of 32 byte (a cache line size)
#define inline_mass_copy(d, s, l) ({\
    unsigned *__d=(unsigned*)d;\
    const unsigned *__s=(const unsigned*)s;\
    unsigned __l=(unsigned)l;\
    unsigned __vr0, __vr1, __vr2, __vr3, __vr4, __vr5, __vr6, __vr7;\
    while (__l>0) {\
        {\
        __vr0=__s[0]; __vr1=__s[1]; __vr2=__s[2]; __vr3=__s[3];\
        __vr4=__s[4]; __vr5=__s[5]; __vr6=__s[6]; __vr7=__s[7];}\
        __s += 8;\
        __d[0]=__vr0; __d[1]=__vr1; __d[2]=__vr2; __d[3]=__vr3;\
        __d[4]=__vr4; __d[5]=__vr5; __d[6]=__vr6; __d[7]=__vr7;\
        __d += 8;\
        __l -= sizeof(unsigned)*8;\
    } })

#define SWBREAK() do {               \
                __asm__ __volatile__ ("sdbbp;"); \
        } while(0)
    

#endif //INLINE_UTIL_H


