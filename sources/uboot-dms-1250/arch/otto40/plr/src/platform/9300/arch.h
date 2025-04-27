
/* Bus Traffic Generator*/
#define LX0_BTG_BASE_ADDR	(0xB8144000)
#define LX1_BTG_BASE_ADDR	(0xB800A000)
#define LX2_BTG_BASE_ADDR	(0xB8018000)

/* switch spi-nor 4byte address*/
#define SWITCH_4B_ADDR_MODE()

/* lplr */
#define _lplr_soc_t  (*(soc_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))
#define _lplr_bios (*(basic_io_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))
#define _lplr_header (_lplr_bios.header)

/* sram init*/
#define SRAM_CTRL_INIT         \
        li  t0, 0xB8001300; \
        lui t1, %hi(OTTO_SRAM_START); \
        lui t3, 0x1FFF; \
        and t1, t1, t3; \
        ori t1, t1, 0x1; /*Set (Un)mapping enable bit*/ \
        li  t3, 0xB8004000; \
        li  t2, OTTO_SRAM_SIZE_CFG; \
        sw  t1, 0(t0); /*unmapping*/ \
        sw  t2, 4(t0); /*unmapping size of default sram controller setting*/ \
        sw  t1, 0(t3); /*mapping*/ \
        sw  t2, 4(t3); /*mapping*/ \
        lui t1, 0x0003;            \
        sw  t1, 8(t3);
