#ifndef TLB_H
#define TLB_H

#include <soc.h>


#define TLB_ASID_SHIFT          (6)
#define TLB_ASID_MASK           (0x3F<<TLB_ASID_SHIFT)
#define TLB_INDEX_SHIFT         (8)

#ifndef __ASSEMBLER__
typedef union {
    struct {
        unsigned int vpn:20;
        unsigned int asid:6;
        unsigned int mbz:6;
    } f;
    unsigned int v;
} tlb_entry_hi_t;

typedef union {
    struct {
        unsigned int pfn:20;
        unsigned int n:1;
        unsigned int d:1;
        unsigned int v:1;
        unsigned int g:1;
        unsigned int mbz:8;
    } f;
    unsigned int v;
} tlb_entry_lo_t;

typedef union {
    struct {
        unsigned int p:1;
        unsigned int mbz0:17;
        unsigned int index:6;
        unsigned int mbz1:8;
    } f;
    unsigned int v;
} tlb_index_t;



#define TLB_SET_ENTRY_HI(entry_hi) ({\
    unsigned int __hi=(entry_hi);\
    __asm__  __volatile__  (\
        "mtc0 %0, " TO_STR(CP0_ENTRYHI) ASM_NL\
        : /* no output */\
        : "r" (__hi)\
    );\
})

#define TLB_SET_ENTRY(idx, entry_hi, entry_lo) ({\
    unsigned int __hi=(entry_hi), __lo=(entry_lo); \
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__ __volatile__  (\
        "mtc0 %0, " TO_STR(CP0_INDEX) ASM_NL\
        "mtc0 %1, " TO_STR(CP0_ENTRYHI) ASM_NL\
        "mtc0 %2, " TO_STR(CP0_ENTRYLO) ASM_NL\
        "tlbwi" ASM_NL\
        : /* no output */\
        : "r" (__idx.v), "r" (__hi), "r" (__lo)\
    );\
})

#define TLB_SET_RANDOM(entry_hi, entry_lo) ({\
    unsigned int __hi=(entry_hi), __lo=(entry_lo); \
    tlb_index_t __idx;\
    __asm__ __volatile__  (\
        "mtc0 %1, " TO_STR(CP0_ENTRYHI) ASM_NL\
        "mtc0 %2, " TO_STR(CP0_ENTRYLO) ASM_NL\
        "tlbwr" ASM_NL\
        "mfc0 %0, " TO_STR(CP0_INDEX) ASM_NL\
        : "=r" (__idx.v)\
        : "r" (__hi), "r" (__lo)\
    );\
    __idx.f.index;\
})

#define TLB_GET_ENTRY(idx, entry_hi, entry_lo) ({\
    unsigned int *__entry_hi=(entry_hi), *__entry_lo=(entry_lo), __hi, __lo;\
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__ __volatile__  (\
        "mtc0 %2, " TO_STR(CP0_INDEX) ASM_NL\
        "tlbr" ASM_NL\
        "mfc0 %0, " TO_STR(CP0_ENTRYHI) ASM_NL\
        "mfc0 %1, " TO_STR(CP0_ENTRYLO) ASM_NL\
        : "=r" (__hi), "=r" (__lo)\
        : "r" (__idx.v)\
    );\
    *__entry_hi=__hi;\
    *__entry_lo=__lo;\
})

#define TLB_GET_ENTRY_HI(idx, entry_hi) ({\
    unsigned int *__entry_hi=(entry_hi), __hi;\
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__ __volatile__  (\
        "mtc0 %1, " TO_STR(CP0_INDEX) ASM_NL\
        "tlbr" ASM_NL\
        "mfc0 %0, " TO_STR(CP0_ENTRYHI) ASM_NL\
        : "=r" (__hi)\
        : "r" (__idx.v)\
    );\
    *__entry_hi=__hi;\
})


#define TLB_GET_BADVADDR() ({\
    unsigned int __ret;\
    __asm__  __volatile__  (\
        "mfc0 %0, " TO_STR(CP0_BADVADDR) ASM_NL\
        : "=r" (__ret)\
    );\
    __ret;\
})

#define TLB_SET_VALID(idx, valid_bit) ({\
    tlb_entry_lo_t __lo;\
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__  __volatile__  (\
        "mtc0 %1, " TO_STR(CP0_INDEX) ASM_NL\
        "tlbr" ASM_NL\
        "mfc0 %0, " TO_STR(CP0_ENTRYLO) ASM_NL\
        : "=r" (__lo.v)\
		: "r" (__idx.v)\
    );\
	__lo.f.v=valid_bit;\
    __asm__  __volatile__  (\
        "mtc0 %0, " TO_STR(CP0_ENTRYLO) ASM_NL\
        "tlbwi" ASM_NL\
        : : "r" (__lo.v)\
    );\
})


#define TLB_GET_CONTEXT() ({\
    unsigned int __ret;\
    __asm__  __volatile__  (\
        "mfc0 %0, " TO_STR(CP0_CONTEXT) ASM_NL\
        : "=r" (__ret)\
    );\
    __ret;\
})
#endif //__ASSEMBLER__


#endif //TLB_H

