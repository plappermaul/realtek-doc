OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
    . = RESET_ENTRY;
    .reset_entry : AT(LMA_BASE) { 
        *(.reset_entry); 
        . = RESET_ENTRY + STRU_BIAS;
        *(.soc_stru_p); 

        hook_func_start = .;
        *(.hook_func.0);
        *(.hook_func.1);
        *(.hook_func.2);
        *(.hook_func.3);
        *(.hook_func.4);
        *(.hook_func.5);
        *(.hook_func.6);
        *(.hook_func.7);
        *(.hook_func.8);
        *(.hook_func.9);
        *(.hook_func.10);
        *(.hook_func.11);
        *(.hook_func.12);
        *(.hook_func.13);
        *(.hook_func.14);
        *(.hook_func.15);
        *(.hook_func.16);
        *(.hook_func.17);
        *(.hook_func.18);
        *(.hook_func.19);
        *(.hook_func.20);
        *(.hook_func.21);
        *(.hook_func.22);
        *(.hook_func.23);
        *(.hook_func.24);
        *(.hook_func.25);
        *(.hook_func.26);
        *(.hook_func.27);
        *(.hook_func.28);
        *(.hook_func.29);
        *(.hook_func.30);
        *(.hook_func.31);
        *(.hook_func.32);
        *(.hook_func.99);
        hook_func_end = .;
        *(.uncache_unswapable_area_small);
        . = RESET_ENTRY + TLB_MISS_ISR;
        *(.isr_tbl_miss); 
        *(.uncache_unswapable_area);
        . = ALIGN(4);
    }
    lma_of_uncache = LOADADDR(.);
    
    . = . & ~20000000;
    .unswapable_area : AT(lma_of_uncache) {
        *(.unswapable_area);
        . = ALIGN(4);
        *(.data); 
        . = ALIGN(4);
        *(.sdata);
        . = ALIGN(4);
    }
    
    start_of_bss = .;
    .comm  :  {
        *(.sbss); 
        *(.bss);
    }
    
    . = ALIGN(4);
    end_of_bss = .;
    
    .recycle_area : AT(LOADADDR(.comm)) { 
        *(.recycle_area);
    }
    
    . = end_of_bss + STACK_SIZE;
    . = ALIGN(4096);
    stack_init_p = .;
    .text : {
        *(.text);
    }
    .rodata : {
        *(.rodata);
    }
}
