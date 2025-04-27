#include "init_level.h"

void INIT_LVL_MIPS32
run_init_level(const init_table_entry_t *lplr_init_table_start, const init_table_entry_t *lplr_init_table_end,
    const init_table_entry_t *plr_init_table_start, const init_table_entry_t *plr_init_table_end) {
    const init_table_entry_t *lp=lplr_init_table_start, *le=lplr_init_table_end;
    const init_table_entry_t *pp=plr_init_table_start, *pe=plr_init_table_end;

    while ((lp!=le)||(pp!=pe)) {
        if ((lp!=le) && (pp!=pe)) {
            if (lp->level<=pp->level) {
                (*lp->fn)();
                lp++;
            } else {
                (*pp->fn)();
                pp++;
            }
        } else if (lp!=le) {
            (*lp->fn)();
            lp++;
        } else {
            //(pp!=pe)
            (*pp->fn)();
            pp++;
        }
    }
}
