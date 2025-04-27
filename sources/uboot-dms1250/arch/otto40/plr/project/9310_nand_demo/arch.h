// UBOOT relative definition
#define MAX_UBOOT_SIZE         (0x80000)
#define ECC_DECODED_UBOOT_ADDR (0x83f00000)
#define NEW_STACK_AT_DRAM      (ECC_DECODED_UBOOT_ADDR-32)
#define UBOOT_DECOMP_ADDR      (0x80100000)

// for BTG cli
#define BTGLX0_BASE            (LX0_BTG_BASE_ADDR)
#define BTGLX1_BASE            (LX1_BTG_BASE_ADDR)
#define BTGLX2_BASE            (0)
