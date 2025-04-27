#include <util.h>

void UTIL_MIPS16
traversal_init_table() {
	init_table_entry_t *pp = _soc_header.init_func_list;
	init_table_entry_t *pe = _soc_header.end_of_init_func_list;
    
	while (pp!=pe) {
		(*pp->fn)();
		pp++;
	}

	while(1) ; // should never return
}
