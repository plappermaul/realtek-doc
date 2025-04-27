OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
    . = ENTRY;
    start_of_plr = .;
    _gp = ENTRY + 0x8000;
    .reset_entry : AT(LMA_BASE) { 
        *(.entry); 
        *(.reset_entry); 
    }

    . = ENTRY + HEADER_OFFSET;
    .front : {
        *(.soc_stru);
        
        start_of_parameters0 = .;
        *(.parameters);
        end_of_parameters0 = .;
    }
    .sdata : {
        *(.sdata.soc_stru); 
        
        start_of_parameters1 = .;
        *(.sdata.parameters);
        end_of_parameters1 = .;

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
        *(.init_func_level.33);
        *(.init_func_level.34);
        *(.init_func_level.35);
        *(.init_func_level.36);
        *(.init_func_level.37);
        *(.init_func_level.38);
        *(.init_func_level.99);
        end_of_init_func_table = .;
        *(.init_func_level.ending_symb);

        start_of_symble_table = .;
        *(.symbol_table);
        end_of_symble_table = .;
        *(.symbol_table.ending_symb);

        mc_var_begin = .;
        *(.mc_var);
        mc_var_end = .;

        *(.sdata);
        . = ALIGN(4);
        start_of_sbss = .;
        *(.sbss);
        . = ALIGN(4);
        end_of_sbss = .;
        
        *(.unswapable_rodata);
        . = ALIGN(4);
        *(.reginfo);
    }
    end_of_sdata = .;
    
    .rel.dyn : { *(.rel.dyn);  }

    .unswapable_text : { *(.unswapable_text); }
    . = ALIGN(4);

    .data : {
        *(.data);
        start_of_bss = .;
        *(.bss);  
        . = ALIGN(4);
        end_of_bss = .;
    }
    farthest_stack_position = .;
    
    . = ALIGN(4096); 
    big_buffer_plr = .;
   
    . = . + 4096;
    mapped_physical_sram_start = .;
    
    .recycle_text : { 
        *(.recycle_text);
    }
    . = ALIGN(4);
    .recycle_data : { 
        *(.recycle_data);
    }
    . = ALIGN(4);
    .spi_nand_probe_func : {
        start_of_spi_nand_probe_func = .;
		*(.spi_nand_probe_func);
        end_of_spi_nand_probe_func = .;
    }

    physical_sram_size = . - ENTRY;
    
    . = ALIGN(4096);
    lma_offset_before_mapped_area = . - ENTRY;
    lma_mapping_start = LMA_BASE + lma_offset_before_mapped_area;
     
    . = KSEG2_BASE;
    start_of_mapping_area = .;
    .rodata : AT (lma_mapping_start) {
        start_of_ddr_probe_func = .;
        *(.ddr_probe_func);
        end_of_ddr_probe_func = .;
        
        *(.rodata);
        *(.rodata.*);
        *(.ro);
    }
    
    . = ALIGN(4);
    .text : {
        *(.text);
    }

    . = ALIGN(4096);
    .pge_align : {
        *(.pge_align);
    }

    .cli_tables : {
        start_of_cli_node = .;
        *(.cli_node);
        end_of_cli_node = .;
        start_of_cli_mp_node = .;
        *(.cli_mp_node);
        end_of_cli_mp_node = .;
        start_of_cli_tail = .;
        *(.cli_tail);
        end_of_cli_tail = .;
        start_of_cli_help = .;
        *(.cli_help);
        end_of_cli_help = .;
    }
    end_of_mapping_area = .;

    end_of_plr = .;
}


