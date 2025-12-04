#ifndef SYMB_DEFINE_H
#define SYMB_DEFINE_H

#include <soc.h>

#if defined(CONFIG_UNDER_UBOOT)
#else
#include <inline_util.h>
#endif

#define symb_pdefine(name, k, p) symbol_table_entry_t CL(__ste_p_ ## name ## _) \
    __attribute__ ((section (".symbol_table"))) = {.key.id = k, .v.pvalue = p}

#define symb_fdefine(k, func) symbol_table_entry_t CL(__ste_f_ ## func ## _) \
    __attribute__ ((section (".symbol_table"))) = {.key.id = k, .v.pvalue = func}

#define symb_idefine(name, k, i) symbol_table_entry_t CL(__ste_p_ ## name ## _) \
    __attribute__ ((section (".symbol_table"))) = {.key.id = k, .v.ivalue = i}

#define RESERVED_SYMB_ID    0xfffffff8

#define MARK_SYMB_TABLE_ENDING_SYMB symbol_table_entry_t __ending_symb_of_symb_table __attribute__ ((section (".symbol_table.ending_symb" ))) = {.key.id=ENDING_SYMB_ID, .v.ivalue=0}
#define RESERVE_SYMB_TABLE_ENTRY symbol_table_entry_t __ending_symb_of_symb_table __attribute__ ((section (".symbol_table" ))) = {.key.id=RESERVED_SYMB_ID, .v.ivalue=0}

/* For symbol table retrive mechanisms. */
const symbol_table_entry_t *
symb_retrive(u32_t key,
    const symbol_table_entry_t *symb_list,
    const symbol_table_entry_t *list_end);
typedef struct {
    unsigned int symb;
    void *value_to_set;
} symb_retrive_entry_t;
u32_t
symb_retrive_list(const symb_retrive_entry_t *list,
    const symbol_table_entry_t *symb_list,
    const symbol_table_entry_t *list_end);


#endif //SYMB_DEFINE_H
