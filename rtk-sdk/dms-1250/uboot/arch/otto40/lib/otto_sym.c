#include <soc.h>

#if defined(CONFIG_UNDER_UBOOT)
#include <asm/symb_define.h>
#else
#include <symb_define.h>
#endif

#ifndef SECTION_SYMACCESS
    #define SECTION_SYMACCESS SECTION_RECYCLE
#endif

SECTION_SYMACCESS const symbol_table_entry_t *
symb_retrive(u32_t key, const symbol_table_entry_t *symb_list, const symbol_table_entry_t *list_end) {
    const symbol_table_entry_t *p=symb_list;
    while (p!=list_end) {
        if (p->key.id==key) return p;
        if (p->key.id==ENDING_SYMB_ID) break;
        ++p;
    }
    return VZERO;
}

SECTION_SYMACCESS u32_t 
symb_retrive_list(const symb_retrive_entry_t *list,
    const symbol_table_entry_t *symb_list,
    const symbol_table_entry_t *list_end) {
    if (list==VZERO) return 0;
    u32_t c=0;
    while (list->symb!=ENDING_SYMB_ID) {
        const symbol_table_entry_t *s=symb_retrive(list->symb, symb_list, list_end);
        if (s!=VZERO) {
            *(u32_t*)list->value_to_set=s->v.ivalue;
            ++c;
        }
        ++list;
    }
    return c;
}
