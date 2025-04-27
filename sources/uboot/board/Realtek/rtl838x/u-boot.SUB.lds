OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)
SECTIONS
{
  /* set to uncached address */
  .ucflash (OTTO_FLASH_BASE | 0x20000000) :
  AT(OTTO_FLASH_BASE) {
    arch/mips/cpu/mips4kec/start.o(.text*)
    *(preloader.uc)
  }

  .sram (OTTO_SRAM_BASE) :
  AT(ALIGN((LOADADDR(.ucflash) + SIZEOF(.ucflash)), 4)) {
    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o(.text*)
    . = ALIGN(4);
    *(.flash_text)
    . = ALIGN(4);
    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o(.rodata*)
    . = ALIGN(4);
    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o(.data*)
    . = ALIGN(4);
    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o(.sdata*)
  }
  preloader_sram_start = LOADADDR(.sram);
  preloader_sram_size  = SIZEOF(.sram);

  . = ALIGN(4);
  .sram.bss :
  AT(0x0) {
    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o(.sbss*)
    . = ALIGN(4);
    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o(.bss*)
  }
  preloader_bss_start = ADDR(.sram.bss);
  preloader_bss_size  = SIZEOF(.sram.bss);

  .dram (OTTO_UBOOT_BASE) :
  AT(ALIGN((LOADADDR(.sram) + SIZEOF(.sram)), 4)) {
    *(EXCLUDE_FILE (arch/mips/cpu/mips4kec/start.o
                    arch/mips/cpu/mips4kec/rtl838x/librtl838x.o)
                   .text*)

    . = ALIGN(4);
    *(EXCLUDE_FILE (arch/mips/cpu/mips4kec/rtl838x/librtl838x.o) .data*)

    . = ALIGN(4);
    *(EXCLUDE_FILE (arch/mips/cpu/mips4kec/rtl838x/librtl838x.o) .sdata*)

    . = ALIGN(4);
    *(EXCLUDE_FILE (arch/mips/cpu/mips4kec/rtl838x/librtl838x.o) .rodata*)

    . = ALIGN(4);
	  __u_boot_cmd_start = .;
    *(.u_boot_cmd)
	  __u_boot_cmd_end = .;

    . = ALIGN(4);
    *(.rel.dyn)
  }
  uboot_start = LOADADDR(.dram);
  uboot_size  = SIZEOF(.dram);

  . = ALIGN(4);
  uboot_end_data = .;
  .sbss :
  AT(0x100000) {
    *(EXCLUDE_FILE (arch/mips/cpu/mips4kec/rtl838x/librtl838x.o) .sbss*)
  }

	. = ALIGN(4);
	.bss :
  AT(0x200000) {
    *(EXCLUDE_FILE (arch/mips/cpu/mips4kec/rtl838x/librtl838x.o) .bss*)
  }
  uboot_end = .;
}
