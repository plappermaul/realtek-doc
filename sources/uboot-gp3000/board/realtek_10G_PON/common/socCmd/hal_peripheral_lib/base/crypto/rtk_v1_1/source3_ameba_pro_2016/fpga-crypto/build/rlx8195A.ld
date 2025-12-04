

ENTRY(Reset_Handler)


MEMORY
{
/*  ROM (rx) : ORIGIN = 0x00000000, LENGTH = 1048576 */
  ROM_B (rx) : ORIGIN = 0x00000000, LENGTH = 327680
  ROM_C (rx) : ORIGIN = 0x00050000, LENGTH = 720716  

  TCM (rwx) : ORIGIN = 0x1FFF0000, LENGTH = 65536
  BD_RAM (rwx)  : ORIGIN = 0x10000000, LENGTH = 458752
/*  BD_RAM (rwx)  : ORIGIN = 0x10000000, LENGTH = 393216
  BF_RAM (rwx)	: ORIGIN = 0x10060000, LENGTH = 65536 */
  SDRAM_RAM (rwx)  : ORIGIN = 0x30000000, LENGTH = 2M
}



SECTIONS
{
  .rom.text :
  {
    KEEP(*(.vectors_table))
	KEEP(*(.vectors_table_text))
	KEEP(*(.hal.rom.text*))
	KEEP(*(.mon.rom.text*))
	KEEP(*(.timer.rom.text*))
	KEEP(*(.infra.rom.text*))
	KEEP(*(.wlan.rom.text*))
	KEEP(*(.apple.rom.text*))
	KEEP(*(.libc.rom.text*))
  } > ROM_B

  .rom.rodata :
  {
    KEEP(*(.hal.rom.rodata*))
    KEEP(*(.mon.rom.rodata*))
    KEEP(*(.halop.rom.rodata*))
    KEEP(*(.wlan.rom.rodata*))
    KEEP(*(.apple.rom.rodata*))
    KEEP(*(.libc.rom.rodata*))
    /* to compatiable with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
    KEEP(*(.rom.rodata*))
     __rom_b_cut_end__ = .;
  } > ROM_B

 .cutc.rom.text :
  {
	KEEP(*(.cutc.rom.text*))
  } > ROM_C

 .cutc.rom.rodata :
  {
	KEEP(*(.cutc.rom.rodata*))
  } > ROM_C

  .vector_table  :
  {
  	KEEP(*(.ram_dedecated_vector_table*))
  } > BD_RAM
  	
  .user_vector_table  :
  {
  	KEEP(*(.ram_user_define_irq_table*))
  } > BD_RAM

  .user_data_table  :
  {
  	KEEP(*(.ram_user_define_data_table*))
  } > BD_RAM

  .rom.bss :
  {
    __rom_bss_start__ = .;
    KEEP(*(.hal.ram.bss*))
    KEEP(*(.timer2_7_vector_table.data*))
    KEEP(*(.infra.ram.bss*))
    KEEP(*(.mon.ram.bss*))
    KEEP(*(.wlan_ram_map*))
    KEEP(*(.libc.ram.bss*))
    __rom_bss_end__ = .;
  } > BD_RAM

  .ram.start.table :
  {
      __ram_image1_text_start__ = .;
    __ram_start_table_start__ = .;
    KEEP(*(SORT(.start.ram.data*)))
    __ram_start_table_end__ = .;
    
  } > BD_RAM


/*  Add . to assign the start address of the section,                                     * 
  *  to prevent the change of the start address by ld doing section alignment */  
    .ram_image1.text . : 
  {
  	/* these 4 sections is used by ROM global variable */
  	/* Don't move them and never add RAM code variable to these sections */
    __ram_image1_data_start__ = .;
    __image1_validate_code__ = .;
    KEEP(*(.image1.validate.rodata*))
    KEEP(*(.infra.ram.data*))
    KEEP(*(.timer.ram.data*))
    KEEP(*(.cutb.ram.data*))
    KEEP(*(.cutc.ram.data*))
    KEEP(*(.hal.ram.data*))
    __image1_bss_start__ = .;
    *(.hal.flash.data*)    
    *(.hal.sdrc.data*)    
    *(.hal.gpio.data*)    
    *(.fwu.data*)    
    __image1_bss_end__ = .;
    __ram_image1_data_end__ = .;    
    *(.hal.ram.text*)    
    *(.hal.flash.text*)    
    *(.hal.sdrc.text*)    
    *(.infra.ram.text*)
    *(.hal.gpio.text*)
    *(.fwu.text*)    
    __ram_image1_text_end__ = .;
  } > BD_RAM

  .image2.start.table :
  {
    __ram_image2_text_start__ = .;
    __image2_entry_func__ = .;
    KEEP(*(SORT(.image2.ram.data*)))
    __image2_validate_code__ = .;
    KEEP(*(.image2.validate.rodata*))    
  } > BD_RAM

  .ram_image2.text :
  {
    *(.infra.ram.start*)
    *(.mon.ram.text*)
    *(.wifi.ram.text*)
    *(.img2.ram.text*)
    *(.wifi.ram.data*)
    *(.img2.ram.data*)
    *(.text .text.* *.text .gnu.linkonce.t.*) 
    *(.ramfunc .ramfunc.*);    
  } > BD_RAM

  .ram.data : 
  {
    __data_start__ = .;
    *(.rodata .rodata* .gnu.linkonce.r.*)
    *(.fwu.rodata*)
    *(.data .data.* *.data);
    
    __data_end__ = .;
  	__ram_image2_text_end__ = .;    

	    *(.bfsram.data*)
	    *(.bdsram.data*)

        . = ALIGN(4);

                __data_start__ = .;
        _sdata = .;
        . = ALIGN(4);
        *(vtable)

        . = ALIGN(4);
	    *(.glue_7t) *(.glue_7)
        *(.ARM.extab* .gnu.linkonce.armextab.*)


        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

	 /* init data */
	. = ALIGN(4);
	KEEP(*(.init))

        /* init data */
        . = ALIGN(4);
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);

	/* .ctors */
        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP(*crtbegin?.o(.ctors))
        KEEP(*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        KEEP(*(.ctors))

	 /* finit data */
	. = ALIGN(4);
	KEEP(*(.fini))


        . = ALIGN(4);
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

	/* .dtors */
        . = ALIGN(4);
        KEEP (*crtbegin.o(.dtors))
        KEEP(*crtbegin?.o(.dtors))
        KEEP(*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*crtend.o(.dtors))
        KEEP(*(.dtors))

	KEEP(*(.eh_frame*))

	KEEP(*(.jcr*))
  	
  } > BD_RAM

      /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > BD_RAM
    PROVIDE_HIDDEN (__exidx_end = .);
    __etext = .;
    _sidata = .;


    /* .stack section doesn't contains any symbols. It is only
     * used for linker to reserve space for the main stack section
     * WARNING: .stack should come immediately after the last secure memory
     * section.  This provides stack overflow detection. */
    .stack (NOLOAD):
    {
        __StackLimit = .;
        *(.stack*);
        . += 0x8000 - (. - __StackLimit);
    } > BD_RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ADDR(.stack) + SIZEOF(.stack);
    __StackLimit = ADDR(.stack);
    PROVIDE(__stack = __StackTop);


    /* .bss section which is used for uninitialized data */
    .bss  (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = . ;
        _szero = .;
        _start_bss = .;
    
    *(.bss*)
    *(COMMON)
    *(.bdsram.data*)
        *(.sdio.rom.bss*)
        
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
        __bss_end__ = .;
        _end_bss = . ;

  } > BD_RAM

  
  .boot_data : 
  {
    __boot_data_start__ = .;
    __boot_data_end__ = .;
    
  } > BD_RAM


    .heap (NOLOAD):
    {
        __end__ = .;
        PROVIDE(end = __end__);
        *(.heap*);
        . += (ORIGIN(BD_RAM) + LENGTH(BD_RAM) - .);
        __HeapLimit = .;
    } > BD_RAM
    PROVIDE(__heap_size = SIZEOF(.heap));
    PROVIDE(__mbed_sbrk_start = ADDR(.heap));
    PROVIDE(__mbed_krbs_start = ADDR(.heap) + SIZEOF(.heap));

/*
  .bf_data : 
  {
    __buffer_data_start__ = .;
    *(.bfsram.data*)
    __buffer_data_end__ = .;
    
  } > BF_RAM
*/


}


