OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
    . = ENTRY;
    start_of_lplr = .;
    .reset_entry : AT(LMA_BASE) { 
        *(.entry); 
        *(.reset_entry); 
    }

    . = ENTRY + HEADER_OFFSET;
    .soc_stru : { 
        *(.soc_stru); 

        start_of_nand_spi_probe_func = .;
		*(.nand_spi_probe_func);
        end_of_nand_spi_probe_func = .;
        *(.reginfo);

    }
    
    . = ALIGN(8);
    .tables : {
        start_of_init_func_table = .;
        *(.init_func_level.0);
        *(.init_func_level.1);
        *(.init_func_level.2);
        *(.init_func_level.3);
        *(.init_func_level.4);
        *(.init_func_level.5);
        *(.init_func_level.6);
        *(.init_func_level.7);
        *(.init_func_level.8);
        *(.init_func_level.9);
        *(.init_func_level.10);
        *(.init_func_level.11);
        *(.init_func_level.12);
        *(.init_func_level.13);
        *(.init_func_level.14);
        *(.init_func_level.15);
        *(.init_func_level.16);
        *(.init_func_level.17);
        *(.init_func_level.18);
        *(.init_func_level.19);
        *(.init_func_level.20);
        *(.init_func_level.21);
        *(.init_func_level.22);
        *(.init_func_level.23);
        *(.init_func_level.24);
        *(.init_func_level.25);
        *(.init_func_level.26);
        *(.init_func_level.27);
        *(.init_func_level.28);
        *(.init_func_level.29);
        *(.init_func_level.30);
        *(.init_func_level.31);
        *(.init_func_level.32);
        *(.init_func_level.99);
        end_of_init_func_table = .;
        *(.init_func_level.ending_symb);

        start_of_symble_table = .;
        *(.symbol_table);
        end_of_symble_table = .;
        *(.symbol_table.ending_symb);

    }
	. = ENTRY + ISR_ENTRY_OFFSET;
	.isr_entry_text : {
		*(.isr_entry_text);
	}
    
    . = ALIGN(4);
    .rodata : {
        *(.rodata*);
        *(.ro);
    }
    
    . = ALIGN(4);
    .text : {
        *(.text);
    }
    end_of_lplr = .;
}

