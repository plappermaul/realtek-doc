#ifndef INIT_LEVEL_H
#define INIT_LEVEL_H
#include <soc.h>

#ifndef INIT_LVL_MIPS32
    #define INIT_LVL_MIPS32  __attribute__((nomips16))
#endif

void INIT_LVL_MIPS32
run_init_level(const init_table_entry_t *lplr_init_table_start, const init_table_entry_t *lplr_init_table_end,
    const init_table_entry_t *plr_init_table_start, const init_table_entry_t *plr_init_table_end);

#endif
