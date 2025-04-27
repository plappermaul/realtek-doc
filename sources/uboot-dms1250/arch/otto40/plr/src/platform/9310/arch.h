
/* Bus Traffic Generator*/
#define LX0_BTG_BASE_ADDR	(0xB800A000)
#define LX1_BTG_BASE_ADDR	(0xB8018000)

/* switch spi-nor 4byte address*/
#define SWITCH_4B_ADDR_MODE()

/* lplr */
#define _lplr_soc_t  (*(soc_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))
#define _lplr_bios (*(basic_io_t*)(OTTO_LPLR_ENTRY+OTTO_HEADER_OFFSET))
#define _lplr_header (_lplr_bios.header)

