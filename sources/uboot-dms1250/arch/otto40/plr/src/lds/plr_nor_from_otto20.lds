OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
SECTIONS
{
    VMA_BASE = SRAM_START;

    .exc_reset
        LMA_BASE :
        AT(LMA_BASE) {
        *(.exc_reset);
    }

    .exc_data0
        (LMA_BASE + HEADER_OFFSET):
        {
        *(.sdata.soc_stru);

        start_of_dram_probe_func = .;
        *(.dram_probe_func);
        end_of_dram_probe_func = .;
    }

    .exc_utlb
        LMA_BASE + 0x100:
        {
        *(.exc_utlb);
    }

    .exc_data1
        :
        {
        *(.exc_data1);
    }

    .exc_rest
        (LMA_BASE + ISR_ENTRY_OFFSET):
        {
        *(.exc_rest)
    }

    . = VMA_BASE + (ALIGN(32) & 0x000fffff);
    .sram_text
        :
        AT(LMA_BASE + (ADDR(.sram_text) & 0x000fffff))
        {
        *(.sram_text);
        *(.unswapable_text);

        *(.recycle_area);
    }

    .data
        :
        AT(LMA_BASE + (ADDR(.data) & 0x000fffff))
        {
        *(.data);
        *(.sdata);
        *(.recycle_data);
    }
    end_of_sram = .;

    . = ALIGN(4);
    start_of_bss = .;
    .bss
        :
        {
        *(.bss);
        *(.sbss);
        *(.scommon);
        *(COMMON);
        . = ALIGN(4);
    }
    end_of_bss = .;
    sp_bottom = .;

    physical_sram_size = . - VMA_BASE;

    . = ALIGN((LOADADDR(.data) + SIZEOF(.data)), 32);
    .parameters
        :
        AT(ADDR(.parameters)) {
        *(.sdata.parameters)
    }

    . = ALIGN(4);
    .nor_spif_probe_func
        :
        AT(ADDR(.nor_spif_probe_func))
        {
        LS_start_of_nor_spif_probe_func = .;
        *(.nor_spif_probe_func);
        LS_end_of_nor_spif_probe_func = .;
    }

    flash_text_begin = .;
    .flash_text
        :
        AT(ADDR(.flash_text))
        {
        *(.text);
        *(.text.*);
        *(.flash_text);
    }
    flash_text_end = .;
    . = ALIGN(4);

    .nor_only_area
        :
        AT(ADDR(.nor_only_area))
        {
        *(.ro);
        *(.rodata);
        *(.rodata.str1.4);

        . = ALIGN(4);
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

        flash_data_begin = .;
        *(.flash_data);
        flash_data_end = .;
        . = ALIGN(16);
    }

    . = ALIGN(16);
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

    . = ALIGN(16);
    .symbol_table : {
        start_of_symble_table = .;
        *(.symbol_table);
        end_of_symble_table = .;
        *(.symbol_table.ending_symb);
    }

    .reginfo : {
        *(.reginfo);
    }

    . = ALIGN(16);
    start_of_next_env = .;
    .next_env : {
        *(.next_env);
    }
    /* . = ALIGN(16); */
    end_of_next_env = .;
}
