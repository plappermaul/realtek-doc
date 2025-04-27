#define project_name 9300_nor_demo
#define platform_name 9300
#define OTTO_LMA_BASE 0x9fc00000
#define OTTO_ENTRY 0x9fc00000
#define OTTO_ENTRY_SYMB plr_S_entry
#define OTTO_PLR_STACK_DEF OTTO_SRAM_START + OTTO_SRAM_SIZE - 8
#define TARGET_OUT
#define EXPORT_SYMB_PROTO_PRINTF 1
#define EXPORT_SYMB_CACHE_OP 1
#define UART_BASE_ADDR 0xb8002000
#define OTTO_DRAM_GEN 2
#define template_name 9300_nor
#define FILTER_OUT_OBJS
#define mirror 9300_nor
#define TARGET_OUT
#define TARGET_LIB libmips34kc.o
#define STANDALONE_UBOOT 1
