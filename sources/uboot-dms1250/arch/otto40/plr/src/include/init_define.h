#ifndef INIT_DEFINE_H
#define INIT_DEFINE_H

#include <soc.h>

#define REG_INIT_FUNC(fn, lvl) \
    init_table_entry_t __init_func_##fn __attribute__ ((section (".init_func_level." #lvl))) = { lvl, (fpv_t*)fn}

#define ENDING_LEVEL    0xffffffff

//#define MARK_INIT_FUNC_ENDING_SYMB init_table_entry_t __ending_symb_of_init_level __attribute__ ((section (".init_func_level.ending_symb" ))) = {ENDING_LEVEL, VZERO}

/*
Convention:

LEVEL   ACTION
------- -------
12(*)   TLB and page table initial, then turn on tlb and its interrupt


24(*)   DDR initial

28(*)   move stack to DDR

*/

#endif

