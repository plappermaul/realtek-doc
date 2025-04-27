#ifndef LPLR_H
#define LPLR_H


#ifdef __ASSEMBLER__

#define UNCACHE_CALL(addr)  \
    la s0, addr;            \
    li s1, 0xA0000000;      \
    or s0, s1;              \
    jalr s0;                \
    nop

#else // __ASSEMBLER__
#endif // __ASSEMBLER__

#endif //LPLR_H


