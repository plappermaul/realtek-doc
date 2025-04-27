#ifndef TLB_H
#define TLB_H

#include <soc.h>

// Cacahe Coherency Attribute Encoding of EntryLo0 and EntryLo1
#define CCAE_UC_NCO             (2)
#define CCAE_CA_NCO_WB_WA	(3)
#define CCAE_CA_CO_WB_WA_RMEX	(4)
#define CCAE_CA_CO_WB_WA_RMSH	(5)
#define CCAE_UC_AC_NCO		(7)

// Valid defintion
#define TLB_INVALID             (0)
#define TLB_VALID               (1)

// TLB Shift
#define TLB_ASID_SHIFT          (0)
#define TLB_ASID_MASK           (0x3F<<TLB_ASID_SHIFT)
#define TLB_INDEX_SHIFT         (0)

// TLB PageMask
#define PAGEMASK_4KB            (0x00000000)
#define PAGEMASK_16KB           (0x00006000)
#define PAGEMASK_64KB           (0x0001E000)
#define PAGEMASK_256KB          (0x0007E000)
#define PAGEMASK_1MB            (0x001FE000)
#define PAGEMASK_4MB            (0x003FE000)
#define PAGEMASK_16MB           (0x01FFE000)
#define PAGEMASK_64MB           (0x07FFE000)
#define PAGEMASK_256MB          (0x1FFFE000)

#ifndef __ASSEMBLER__
typedef union {
    struct {
        unsigned int vpn2:19;
        unsigned int mbz:5;
        unsigned int asid:8;
    } f;
    unsigned int v;
} tlb_entry_hi_t;

typedef union {
    struct {
        unsigned int mbz:6;
        unsigned int pfn:20;
        unsigned int c:3;
        unsigned int d:1;
        unsigned int v:1;
        unsigned int g:1;
    } f;
    unsigned int v;
} tlb_entry_lo_t;

typedef union {
    struct {
        unsigned int p:1;
        unsigned int mbz:25;
        unsigned int index:6;
    } f;
    unsigned int v;
} tlb_index_t;

typedef union {
    struct {
        unsigned int mbz:3;
        unsigned int mask:16;
        unsigned int mbz1:13;
    } f;
    unsigned int v;
} pagemask_t;


#define TLB_GET_PAGEMASK() ({\
    unsigned int __mask;\
    __asm__  __volatile__  (\
        "mfc0 %0, " TO_STR(CP0_PAGEMASK) ASM_NL\
        : "=r" (__mask)\
        : /* no input */\
    );\
    __mask;\
})


#define TLB_SET_PAGEMASK(mask) ({\
    unsigned int __mask=(mask);\
    __asm__  __volatile__  (\
        "mtc0 %0, " TO_STR(CP0_PAGEMASK) ASM_NL\
        : /* no output */\
        : "r" (__mask)\
    );\
})

#define TLB_SET_ENTRY_HI(entry_hi) ({\
    unsigned int __hi=(entry_hi);\
    __asm__  __volatile__  (\
        "mtc0 %0, " TO_STR(CP0_ENTRYHI) ASM_NL\
        : /* no output */\
        : "r" (__hi)\
    );\
})

#define TLB_SET_ENTRY(idx, entry_hi, entry_lo0, entry_lo1) ({\
    unsigned int __hi=(entry_hi), __lo0=(entry_lo0), __lo1=(entry_lo1); \
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__ __volatile__  (\
        "mtc0 %0, " TO_STR(CP0_INDEX) ASM_NL\
        "mtc0 %1, " TO_STR(CP0_ENTRYHI) ASM_NL\
        "mtc0 %2, " TO_STR(CP0_ENTRYLO0) ASM_NL\
        "mtc0 %3, " TO_STR(CP0_ENTRYLO1) ASM_NL\
        "tlbwi" ASM_NL\
        : /* no output */\
        : "r" (__idx.v), "r" (__hi), "r" (__lo0), "r" (__lo1)\
    );\
})

#define TLB_SET_RANDOM(entry_hi, entry_lo0, entry_lo1) ({\
    unsigned int __hi=(entry_hi), __lo0=(entry_lo0), __lo1=(entry_lo1); \
    tlb_index_t __idx;\
    __asm__ __volatile__  (\
        "mtc0 %1, " TO_STR(CP0_ENTRYHI) ASM_NL\
        "mtc0 %2, " TO_STR(CP0_ENTRYLO0) ASM_NL\
        "mtc0 %3, " TO_STR(CP0_ENTRYLO1) ASM_NL\
        "tlbwr" ASM_NL\
        "mfc0 %0, " TO_STR(CP0_INDEX) ASM_NL\
        : "=r" (__idx.v)\
        : "r" (__hi), "r" (__lo0), "r" (__lo1)\
    );\
    __idx.f.index;\
})

#define TLB_GET_ENTRY(idx, entry_hi, entry_lo0, entry_lo1) ({\
    unsigned int *__entry_hi=(entry_hi), *__entry_lo0=(entry_lo0), *__entry_lo1=(entry_lo1), __hi, __lo0, __lo1;\
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__ __volatile__  (\
        "mtc0 %3, " TO_STR(CP0_INDEX) ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
        "tlbr" ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
        "mfc0 %0, " TO_STR(CP0_ENTRYHI) ASM_NL\
        "mfc0 %1, " TO_STR(CP0_ENTRYLO0) ASM_NL\
        "mfc0 %2, " TO_STR(CP0_ENTRYLO1) ASM_NL\
        : "=r" (__hi), "=r" (__lo0), "=r" (__lo1)\
        : "r" (__idx.v)\
    );\
    *__entry_hi=__hi;\
    *__entry_lo0=__lo0;\
    *__entry_lo1=__lo1;\
})

#define _TLB_GET_ENTRY_HI() ({\
    unsigned int __hi;\
    __asm__ __volatile__  (\
        "mfc0 %0, " TO_STR(CP0_ENTRYHI) ASM_NL\
        : "=r" (__hi)\
        : /* no input */\
    );\
    __hi;\
})

#define TLB_GET_ENTRY_HI(idx, entry_hi) ({\
    unsigned int *__entry_hi=(entry_hi), __hi;\
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__ __volatile__  (\
        "mtc0 %1, " TO_STR(CP0_INDEX) ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
        "tlbr" ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
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

#define _TLB_SET_VALID(idx, valid_bit, entryloN) ({\
    tlb_entry_lo_t __lo;\
    tlb_index_t __idx={.v=0};\
    __idx.f.index=idx;\
    __asm__  __volatile__  (\
        "mtc0 %1, " TO_STR(CP0_INDEX) ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
        "tlbr" ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
        "mfc0 %0, " TO_STR(entryloN) ASM_NL\
        : "=r" (__lo.v)\
        : "r" (__idx.v)\
    );\
    __lo.f.v=valid_bit;\
    __asm__  __volatile__  (\
        "mtc0 %0, " TO_STR(entryloN) ASM_NL\
        "nop" ASM_NL\
        "nop" ASM_NL\
        "tlbwi" ASM_NL\
        : : "r" (__lo.v)\
    );\
})

#define TLB_SET_ENTRYLO0_VALID(idx, valid_bit)  _TLB_SET_VALID(idx, valid_bit, CP0_ENTRYLO0)
#define TLB_SET_ENTRYLO1_VALID(idx, valid_bit)  _TLB_SET_VALID(idx, valid_bit, CP0_ENTRYLO1)

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

