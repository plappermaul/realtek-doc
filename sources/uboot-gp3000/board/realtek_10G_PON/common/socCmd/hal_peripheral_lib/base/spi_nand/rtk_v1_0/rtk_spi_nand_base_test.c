
#include "include/ecc_ctrl.h"
#include "include/rtk_spi_nand_ctrl.h"
#include "include/rtk_spi_nand.h"
#include "include/util.h"
#include <common.h>
#include <malloc.h>
#include <asm/mipsregs.h>
#include "include/util.h"

#define  CAUSEB_DC              27
#define  CAUSEF_DC              (_ULCAST_(1)   << 27)

#define VZERO 0

#define SECTION_RO

#define CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND 1

plr_nand_spi_info_t *probe_snand_chip(void);

static int chip_select = 1;

/*Choosing SPI-NAND I/O type
    Definition in nand_spi_ctrl.h
*/

#ifdef NAND_SPI_USE_SIO
SECTION_RO cmd_info_t snand_cmd_info = {
	.w_cmd = SNAND_PROGRAM_LOAD,
	.w_addr_io = SIO_WIDTH,
	.w_data_io = SIO_WIDTH,
	.r_cmd = SNAND_NORMAL_READ,
	.r_addr_io = SIO_WIDTH,
	.r_data_io = SIO_WIDTH,
	._wait_spi_nand_ready = snand_wait_spi_nand_ready
};
#endif


#ifdef NAND_SPI_USE_DIO
SECTION_RO cmd_info_t snand_cmd_info = {
	.w_cmd = SNAND_PROGRAM_LOAD, //Wonbond's program_load only x1 & x4
	.w_addr_io = SIO_WIDTH,
	.w_data_io = SIO_WIDTH,
	.r_cmd = SNAND_FAST_READ_DIO,
	.r_addr_io = DIO_WIDTH,
	.r_data_io = DIO_WIDTH,
	._wait_spi_nand_ready = snand_wait_spi_nand_ready
};
#endif

#ifdef NAND_SPI_USE_DATA_IOx2
SECTION_RO cmd_info_t snand_cmd_info = {
	.w_cmd = SNAND_PROGRAM_LOAD,
	.w_addr_io = SIO_WIDTH,
	.w_data_io = SIO_WIDTH,
	.r_cmd = SNAND_FAST_READ_X2,
	.r_addr_io = SIO_WIDTH,
	.r_data_io = DIO_WIDTH,
	._wait_spi_nand_ready = snand_wait_spi_nand_ready
};
#endif

#ifdef NAND_SPI_USE_QIO
SECTION_RO cmd_info_t snand_cmd_info = {
	.w_cmd = SNAND_PROGRAM_LOAD_X4,
	.w_addr_io = SIO_WIDTH, //address can't be quad
	.w_data_io = QIO_WIDTH,
	.r_cmd = SNAND_FAST_READ_QIO,
	.r_addr_io = QIO_WIDTH,
	.r_data_io = QIO_WIDTH,
	._wait_spi_nand_ready = snand_wait_spi_nand_ready
};
#endif

#ifdef NAND_SPI_USE_DATA_IOx4
SECTION_RO cmd_info_t snand_cmd_info = {
	.w_cmd = SNAND_PROGRAM_LOAD_X4,
	.w_addr_io = SIO_WIDTH,
	.w_data_io = QIO_WIDTH,
	.r_cmd = SNAND_FAST_READ_X4,
	.r_addr_io = SIO_WIDTH,
	.r_data_io = QIO_WIDTH,
	._wait_spi_nand_ready = snand_wait_spi_nand_ready
};
#endif

SECTION_RO plr_nand_spi_info_t snand_info = {
#ifdef NAND_SPI_USE_2K
	.man_id = SNAND_MANUFACTURER_ID,
	.dev_id = SNAND_DEVICE_ID_W25N01GV,
	.num_block = 1024,
	.num_page_per_block = 64,
	.page_size = PAGE_SIZE_2K,
#else
	.man_id = ETRON_MANUFACTURER_ID,
	.dev_id = ETRON_DEVICE_ID_EM73E044SNA,
	.num_block = 2048,
	.num_page_per_block = 	64,
	.page_size = PAGE_SIZE_4K,
#endif
	._probe = probe_snand_chip,
	._erase_block =	snand_block_erase,
	._pio_write = snand_pio_write_data,
	._pio_read = snand_pio_read_data,
	._page_read = snand_page_read,
	._page_write =	snand_page_write,
	._dma_read = snand_dma_read_data,
	._dma_write = snand_dma_write_data,
	._page_read_ecc = snand_page_read_with_ecc_decode,
	._page_write_ecc = snand_page_write_with_ecc_encode,
	._ecc_encode = ecc_encode_bch_6t,
	._ecc_decode = ecc_decode_bch_6t,
	._chk_ecc_error_sts = ecc_check_decode_status
};

#define BSP_PS_I_SPIC_BASE            0xB8143000 //For 8196F MIPS24K
#define SPIC_SSIENR 0x8
#define SPIC_SER 0x10

void
SNAND_ChipSelect(unsigned int chip_select
)
{
    REG32(BSP_PS_I_SPIC_BASE + SPIC_SSIENR) &= ~(1 << 0);       // SSIENR
                                                                // bit[0]=0
    REG32(BSP_PS_I_SPIC_BASE + SPIC_SER) &= ~(3 << 0);  // clear bit[1:0]
    // REG32(BSP_PS_I_SPIC_BASE+SPIC_SER)|= (1<<0);//cs=0 is default

    if (chip_select == 1) {
        REG32(BSP_PS_I_SPIC_BASE + SPIC_SER) |= (1 << 1);       // cs=1
    } else {
        REG32(BSP_PS_I_SPIC_BASE + SPIC_SER) |= (1 << 0);       // cs=0
    }

}

int
spinand_mem_cmp(u32 dst1, u32 src1, u32 length1)
{
	u32 i, length;
	u32 dst, src;
	u32 dst_value, src_value;
	u8 error;

	// printf("\nSPIcmp().");
	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	// printf("\ndst=%x,src=%x,length=%x\n",dst,src,length);
	error = 0;
	for (i = 0; i < length; i += 4)
	{
		dst_value = *(volatile unsigned int *) (dst + i);
		src_value = *(volatile unsigned int *) (src + i);
		if (dst_value != src_value)
		{
			printf("\n->%dth data(dst=%x ,src=%x) error\n", i, dst_value, src_value);
			error = 1;
			// return 1;
		}
	}
	if (!error) {
		printf("\n->Compare OK\n");
	}
	return error;
}

SECTION_NAND_SPI unsigned int
snand_get_feature_register(unsigned int feature_addr)
{
	unsigned int w_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(2));
	unsigned int r_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(1));
	unsigned int ret = PIO_RAW_CMD(SNAND_GET_FEATURE, feature_addr, w_io_len, r_io_len);
	return ((ret >> 24) & 0xFF);
}

static int
snand_get_feature_register_cmd(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{

#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbbe <blk_pge_addr>  : SPI-NAND Winbond block erasein hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

	unsigned int value;
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);

	value = snand_get_feature_register(blk_pge_addr);
	printf(" Get feature register addr 0x%x : 0x%x\n",blk_pge_addr, value);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(sngf, 2, 0, snand_get_feature_register_cmd, "sngf <addr>  : SPI-NAND Winbond get feature \n", "");


SECTION_NAND_SPI void
snand_set_feature_register(unsigned int feature_addr, unsigned int setting)
{
	unsigned int w_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(3));
	PIO_RAW_CMD(SNAND_SET_FEATURE, ((feature_addr << 8) | setting), w_io_len, SNF_DONT_CARE);
}

#ifdef NAND_SPI_USE_QIO
SECTION_NAND_SPI void
snand_quad_enable(void)
{
	unsigned int feature_addr = 0xA0;
	unsigned int value = 0xFD;
	value &= snand_get_feature_register(feature_addr);
	snand_set_feature_register(feature_addr, value);
}
#endif

SECTION_NAND_SPI void
snand_block_unprotect(void)
{
	unsigned int feature_addr = 0xA0;
	unsigned int value = 0x00;
	value &= snand_get_feature_register(feature_addr);
	snand_set_feature_register(feature_addr, value);
}

SECTION_NAND_SPI void
snand_set_buffer_mode(void)
{
	unsigned int feature_addr = 0xB0;
	unsigned int value = snand_get_feature_register(feature_addr);
	value |= (1 << 3);
	snand_set_feature_register(feature_addr, value);
}

SECTION_NAND_SPI void
snand_disable_on_die_ecc(void)
{
	unsigned int feature_addr = 0xB0;
	unsigned int value = snand_get_feature_register(feature_addr);
	value &= ~(1 << 4);
	snand_set_feature_register(feature_addr, value);
}


SECTION_NAND_SPI void
snand_wait_spi_nand_ready(void)
{
	unsigned int feature_addr = 0xC0;
	unsigned int oip = snand_get_feature_register(feature_addr);

	while ((oip & 0x1) != 0)
	{
		oip = snand_get_feature_register(feature_addr);
	}
}

SECTION_NAND_SPI int
snand_block_erase(unsigned int blk_pge_addr)
{
	write_enable();

	/*
	 * 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address
	 */
	unsigned int w_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(4));
	PIO_RAW_CMD(SNAND_BLOCK_ERASE, blk_pge_addr, w_io_len, SNF_DONT_CARE);
	snand_wait_spi_nand_ready();

	/*
	 * Check Erase Status
	 */
	unsigned int efail = ((snand_get_feature_register(0xC0) >> 2) & 0x1);
	if (1 == efail) {
		printf("  <<ERROR>> Block erase Fail: blk_pge_addr=0x%x\n", blk_pge_addr);
		return -1;
	}
	return 0;
}


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_block_erase_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbbe <blk_pge_addr>  : SPI-NAND Winbond block erasein hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);

	snand_block_erase(blk_pge_addr);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbbe, 2, 0, snand_block_erase_uboot,
           "snwbbe <blk_pge_addr>  : SPI-NAND Winbond block erase \n", "");
#endif


/*
 * PIO Read Less than One Chunk (Less than 2112-Byte)
 * Start from the assigned cache register address (CA=col_addr)
 */
SECTION_NAND_SPI void
snand_pio_read_data(void *ram_addr, unsigned int wr_bytes, unsigned int blk_pge_addr, unsigned int col_addr)
{

#ifdef NAND_SPI_USE_2K
	if (wr_bytes > PAGE_SPARE_SIZE_2K) {
		printf("\nFail => PIO Read Less than One Chunk (Less than 2112 (0x840)-Byte)\n");
		return;
	}
#else
	if (wr_bytes > PAGE_SPARE_SIZE_4K) {
		printf("\nFail => PIO Read Less than One Chunk (Less than 4352 (0x1100)-Byte)\n");
		return;
	}
#endif

	page_data_read_to_cache_buf(blk_pge_addr, &snand_cmd_info);
	PIO_WRITE_READ_DATA(SNAND_READ, col_addr, snand_cmd_info.r_addr_io, snand_cmd_info.r_data_io, wr_bytes, ram_addr);
}

#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_pio_read_data_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{

#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbpior <ram_addr><wr_bytes><blk_pge_addr><col_addr>  : SPI-NAND Winbond pio read in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *ram_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int wr_bytes = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	unsigned int col_addr = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
#endif
	snand_pio_read_data(ram_addr, wr_bytes, blk_pge_addr, col_addr);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbpior, 5, 0, snand_pio_read_data_uboot,
           "snwbpior <ram_addr><wr_bytes><blk_pge_addr><col_addr>  : SPI-NAND Winbond pio read \n", "");
#endif


SECTION_NAND_SPI void
snand_pio_write_data(void *ram_addr, unsigned int wr_bytes, unsigned int blk_pge_addr, unsigned int col_addr)
/*
 * PIO Write Less than One Chunk (Less than 2112-Byte)
 * Start from the assigned cache register address (CA=col_addr)
 */
{
#ifdef NAND_SPI_USE_2K
	if (wr_bytes > PAGE_SPARE_SIZE_2K) {
		printf("\nFail => PIO Write Less than One Chunk  (Less than 2112 (0x840)-Byte)\n");
		return;
	}
#else
	if (wr_bytes > PAGE_SPARE_SIZE_4K) {
		printf("\nFail => PIO Write Less than One Chunk  (Less than 4352 (0x1100)-Byte)\n");
		return;
	}
#endif
	write_enable();
	PIO_WRITE_READ_DATA(SNAND_WRITE, col_addr, snand_cmd_info.w_addr_io,  snand_cmd_info.w_data_io, wr_bytes, ram_addr);
	program_execute(blk_pge_addr, &snand_cmd_info);
}


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_pio_write_data_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]) && (argv[4])) {
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbpiow <ram_addr><wr_bytes><blk_pge_addr><col_addr>  : SPI-NAND Winbond pio write in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *ram_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int wr_bytes = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	unsigned int col_addr = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
#endif

	snand_pio_write_data(ram_addr, wr_bytes, blk_pge_addr, col_addr);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbpiow, 5, 0, snand_pio_write_data_uboot,
           "snwbpiow <ram_addr><wr_bytes><blk_pge_addr><col_addr>  : SPI-NAND Winbond pio write \n", "");
#endif


/*
 * Read More or Less than One Chunk (More or less than 2112-Byte)
 * Start from the initial cache register (CA=0x0000)
 */
SECTION_NAND_SPI void
snand_dma_read_data(void *dma_addr, unsigned int dma_len, unsigned int blk_pge_addr)
{
	dma_read_data(dma_addr, dma_len, blk_pge_addr, &snand_cmd_info);
}


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_dma_read_data_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{

#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]))
   	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbdmar <dma_addr><dma_len><blk_pge_addr>  : SPI-NAND Winbond DMA read in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dma_len = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
#endif

#if 1
	snand_dma_read_data(dma_addr, dma_len, blk_pge_addr);
#else
	if (i = 0; i <=; i++) {
		dma_addr +=;
		snand_dma_read_data(dma_addr, dma_len, blk_pge_addr);
	}
#endif

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbdmar, 4, 0, snand_dma_read_data_uboot,
           "snwbdmar <dma_addr><dma_len><blk_pge_addr>  : SPI-NAND Winbond DMA read \n", "");
#endif


/*
 * Write More or Less than One Chunk (More or less than 2112-Byte)
 * Start from the initial cache register (CA=0x0000)
 */
SECTION_NAND_SPI void
snand_dma_write_data(void *dma_addr, unsigned int dma_len, unsigned int blk_pge_addr)
{
	dma_write_data(dma_addr, dma_len, blk_pge_addr, &snand_cmd_info);
}


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_dma_write_data_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{

#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbdmaw <dma_addr><dma_len><blk_pge_addr>  : SPI-NAND Winbond DMA write in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dma_len = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
#endif

	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);
	snand_dma_write_data(dma_addr, dma_len, blk_pge_addr);
	return CMD_RET_SUCCESS;
}


U_BOOT_CMD(snwbdmaw, 4, 0, snand_dma_write_data_uboot,
           "snwbdmaw <dma_addr><dma_len><blk_pge_addr>  : SPI-NAND Winbond DMA write \n", "");
#endif


/*
 * Read One Chunk (2048-Byte SRC Data + 64-Byte Spare Data)
 * Start from the initial cache register (CA=0x0000)
 */
SECTION_NAND_SPI void
snand_page_read(void *dma_addr, unsigned int blk_pge_addr)
{
#ifdef READ_PERFORMANCE
	unsigned int j;
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif
#ifdef NAND_SPI_USE_2K
	snand_dma_read_data(dma_addr, PAGE_SPARE_SIZE_2K, blk_pge_addr);
#else
	snand_dma_read_data(dma_addr, PAGE_SPARE_SIZE_4K, blk_pge_addr);
#endif
#ifdef READ_PERFORMANCE
	j=read_c0_count();
	printf("read only cpu cp0 count =%d\n", j);
#endif
}

#if 1   // CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_page_read_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if 0
	unsigned int flash_src_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_dest_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
#else
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	// unsigned int dma_len= Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);

	// printf("\ndma_addr =0x%x\n",*dma_addr);
	// printf("\ndma_len =0x%x\n",dma_len);
	printf("\nblk_pge_addrr =0x%x\n", blk_pge_addr);
#endif

	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);
	snand_page_read(dma_addr, blk_pge_addr);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbr, 3, 0, snand_page_read_uboot,
           "snwbr <dma_addr><blk_pge_addr>  : SPI-NAND Winbond DMA 2KB_chunk_read\n", "");

#endif


/*
 * Write One Chunk (2048-Byte SRC Data + 64-Byte Spare Data)
 * Start from the initial cache register (CA=0x0000)
 */
SECTION_NAND_SPI void
snand_page_write(void *dma_addr, unsigned int blk_pge_addr)
{
#ifdef NAND_SPI_USE_2K
	snand_dma_write_data(dma_addr, PAGE_SPARE_SIZE_2K, blk_pge_addr);
#else
	snand_dma_write_data(dma_addr, PAGE_SPARE_SIZE_4K, blk_pge_addr);
#endif
}


#if 1   // CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_page_write_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if 0
	unsigned int flash_src_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int dram_dest_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
#else
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	// unsigned int dma_len= Strtoul((const u8*)(argv[2]), (u8 **)NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);

	// printf("\ndma_addr =0x%x\n",*dma_addr);
	// printf("\ndma_len =0x%x\n",dma_len);
	printf("\nblk_pge_addrr =0x%x\n", blk_pge_addr);
#endif

	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);
	snand_page_write(dma_addr, blk_pge_addr);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbw, 3, 0, snand_page_write_uboot,
           "snwbw <dma_addr><blk_pge_addr>  : SPI-NAND Winbond DMA 2KB_chunk_write\n", "");
#endif

SECTION_NAND_SPI void
snand_page_write_with_ecc_encode(void *dma_addr, unsigned int blk_pge_addr, void *p_eccbuf)
{
	int i = 0, j;
	unsigned int* dma_start = (unsigned int*)dma_addr;
//#define WRITE_ECC_PERFORMANCE
#if 0//def WRITE_ECC_PERFORMANCE
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif
	ecc_encode_bch_6t(dma_addr, p_eccbuf);
#if 0//def WRITE_ECC_PERFORMANCE
	set_c0_cause(CAUSEF_DC);
	j=read_c0_count();
	printf("write ECC cpu cp0 count =%d\n", j);
#endif
#ifdef WRITE_ECC_PERFORMANCE
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif
	snand_page_write(dma_addr, blk_pge_addr);
#ifdef WRITE_ECC_PERFORMANCE
	set_c0_cause(CAUSEF_DC);
	j=read_c0_count();
	printf("write DMA cpu cp0 count =%d\n", j);
#endif

}


SECTION_NAND_SPI void
snand_page_write_with_ecc_encode_buffered(void *dma_addr, unsigned int blk_pge_addr, void *p_eccbuf)
{
	printf("write dma addr = 0x%x, blk_pge_addr = %d\n",(unsigned int)dma_addr, blk_pge_addr);
	u8* temp_page_buffer = 0xa0600000;
#ifdef NAND_SPI_USE_2K
	memcpy(temp_page_buffer, dma_addr, PAGE_SIZE_2K);
#else
	memcpy(temp_page_buffer, dma_addr, PAGE_SIZE_4K);
#endif
	ecc_encode_bch_6t(temp_page_buffer, p_eccbuf);
	snand_page_write(temp_page_buffer, blk_pge_addr);
}


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_dma_write_data_with_ecc_encode_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbwecc <dma_addr><blk_pge_addr><p_eccbuf>  : SPI-NAND Winbond chunk_write with ecc in hex\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	void *p_eccbuf = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
#endif

	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);
	snand_page_write_with_ecc_encode(dma_addr, blk_pge_addr, p_eccbuf);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbwecc, 4, 0, snand_dma_write_data_with_ecc_encode_uboot,
           "snwbwecc <dma_addr><blk_pge_addr><p_eccbuf>  : SPI-NAND Winbond chunk_write with ecc\n", "");
#endif


static int
snand_page_write_with_ecc_encode_continue(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{

#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]))
	{
		/*
		 * Avoid error input format hang
		 */
	} else {
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbweccs <dma_addr><blk_pge_addr><ecc working buffer><length in hex>  : SPI-NAND Winbond continue write with ecc\r\n");
		return CMD_RET_FAILURE;
	}
#endif


#if 1
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	void  *p_eccbuf = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
#endif

	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);

	//calculate page size
	printf("file length = %d\n", length);
	unsigned int i, page_num, src_addr;
#ifdef NAND_SPI_USE_2K
	page_num = (length + PAGE_SIZE_2K_MASK)/PAGE_SIZE_2K;
#else
	page_num = (length + PAGE_SIZE_4K_MASK)/PAGE_SIZE_4K;
#endif
	src_addr = (unsigned int)dma_addr;
	for(i = 0; i < page_num; i++)
	{
		snand_page_write_with_ecc_encode_buffered((void*)src_addr, blk_pge_addr, p_eccbuf);
		blk_pge_addr++;
#ifdef NAND_SPI_USE_2K
		src_addr = src_addr+ PAGE_SIZE_2K;
#else
		src_addr = src_addr+ PAGE_SIZE_4K;
#endif
		udelay(1000000);
	}
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbweccs, 5, 0, snand_page_write_with_ecc_encode_continue,
           "snwbweccs <dma_addr><blk_pge_addr><ecc working buffer><length in hex>  : SPI-NAND Winbond continue write with ecc\n", "")


SECTION_NAND_SPI int
snand_page_read_with_ecc_decode(void *dma_addr, unsigned int blk_pge_addr, void *p_eccbuf)
{
	// whole chunk=2048+64 bytes
	snand_page_read(dma_addr, blk_pge_addr);
	return ecc_decode_bch_6t(dma_addr, p_eccbuf);
}


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
snand_dma_read_data_with_ecc_decode_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbrecc <dma_addr><blk_pge_addr><p_eccbuf>  : SPI-NAND Winbond chunk_read with ecc\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	void  *p_eccbuf = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
#endif
	unsigned int j;
	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);
//#define READ_ECC_PERFORMANCE
#ifdef READ_ECC_PERFORMANCE
	set_c0_cause(CAUSEF_DC);
	write_c0_count(0);
	clear_c0_cause(CAUSEF_DC);
#endif
	snand_page_read_with_ecc_decode(dma_addr, blk_pge_addr, p_eccbuf);
#ifdef READ_ECC_PERFORMANCE
	set_c0_cause(CAUSEF_DC);
	j=read_c0_count();
	printf("cpu cp0 count =%d\n", j);
#endif
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbrecc, 4, 0, snand_dma_read_data_with_ecc_decode_uboot,
           "snwbrecc <dma_addr><blk_pge_addr><p_eccbuf>  : SPI-NAND Winbond chunk_read with ecc\n", "");
#endif


static int
snand_page_read_with_ecc_decode_continue(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
#if CONFIG_AVOID_ERROR_INPUT_HANG_SPI_NAND
	if ((argv[1]) && (argv[2]) && (argv[3]))
	{
		/*
		 * Avoid error input format hang
		 */
	}
	else
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snwbreccs <dma_addr><blk_pge_addr><p_eccbuf><length>  : SPI-NAND Winbond continue read with ecc\r\n");
		return CMD_RET_FAILURE;
	}
#endif

#if 1
	void *dma_addr = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	unsigned int blk_pge_addr = Strtoul((const u8 *) (argv[2]), (u8 **) NULL, 16);
	void  *p_eccbuf = Strtoul((const u8 *) (argv[3]), (u8 **) NULL, 16);
	unsigned int length = Strtoul((const u8 *) (argv[4]), (u8 **) NULL, 16);
#endif

	// snand_dma_write_data(void *dma_addr, unsigned int dma_len,
	// unsigned int blk_pge_addr);
	unsigned int i, page_num, des_addr;
	u8* temp_page_buffer = 0xa0600000;
	des_addr = (unsigned int)dma_addr;
#ifdef NAND_SPI_USE_2K
	page_num = (length + PAGE_SIZE_2K_MASK)/PAGE_SIZE_2K;
#else
	page_num = (length + PAGE_SIZE_4K_MASK)/PAGE_SIZE_4K;
#endif
	for(i = 0; i < page_num; i++)
	{
		snand_page_read_with_ecc_decode((void*)temp_page_buffer, blk_pge_addr, p_eccbuf);
#ifdef NAND_SPI_USE_2K
		memcpy((void*)des_addr, (void*)temp_page_buffer, PAGE_SIZE_2K);
		memset((void*)temp_page_buffer, 0, PAGE_SPARE_SIZE_2K);
		des_addr = des_addr + PAGE_SIZE_2K;
#else
		memcpy((void*)des_addr, (void*)temp_page_buffer, PAGE_SIZE_4K);
		memset((void*)temp_page_buffer, 0, PAGE_SPARE_SIZE_4K);
		des_addr = des_addr + PAGE_SIZE_4K;
#endif
		blk_pge_addr++;
		udelay(1000);
	}
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwbreccs, 5, 0, snand_page_read_with_ecc_decode_continue,
	       "snwbreccs <dma_addr><blk_pge_addr><p_eccbuf><length>  : SPI-NAND Winbond continue read with ecc\n", "");


SECTION_NAND_SPI void
snand_reset(void)
{
	unsigned int w_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(1));
	PIO_RAW_CMD(SNAND_RESET, SNF_DONT_CARE, w_io_len, SNF_DONT_CARE);
}


#if 1
SECTION_NAND_SPI unsigned int
snand_read_id(void)
{

#if 1
	unsigned int dummy = 0x00;
	// JSW:write length = 2byte
	unsigned int w_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(2));
	// JSW:write length = 3byte
	unsigned int r_io_len = IO_WIDTH_LEN(SIO_WIDTH, SNFCMR_LEN(3));

	// #define SNAND_RDID 0x9f
	unsigned int ret = PIO_RAW_CMD(SNAND_RDID, dummy, w_io_len, r_io_len);
	//printf("pio read id data = 0x%x\n", ret);
	printf("\nspi_nand_id =0x%x\n", ((ret >> 8) & 0xFFFFFF));
	return ((ret >> 8) & 0xFFFFFF);
#else
	snand_reset();

	/*
	 * (Step2) Enable CS
	 */
	WAIT_SPI_NAND_CTRLR_RDY();
	cs_high();  /* deactivate CS */
	WAIT_SPI_NAND_CTRLR_RDY();
	cs_low();   /* activate CS */

	unsigned int rdid = snand_read_id();

	printf("\nrdid =0x%x\n", rdid);

	return VZERO;
#endif
}

#endif



SECTION_NAND_SPI plr_nand_spi_info_t *
probe_snand_chip(void)
{
	snand_reset();	// caused BUF=1
	udelay(100);
	unsigned int rdid = snand_read_id();
	udelay(100);

	if ((SNAND_RDID_W25N01GV == rdid) || (SNAND_RDID_W25N01GV_B == rdid) || (MXIC_RDID_M35LF1GE4AB == rdid))
	{
		snand_disable_on_die_ecc();
		udelay(100);
		snand_set_buffer_mode();
		udelay(100);
		snand_block_unprotect();
		udelay(100);

#if 0//def NAND_SPI_USE_DIO
#error
		snand_cmd_info.r_cmd = SNAND_FAST_READ_DIO;
		snand_cmd_info.r_addr_io = DIO_WIDTH;
		snand_cmd_info.r_data_io = DIO_WIDTH;
#endif
		printf("\nFound recognized ID, rdid=0x%x\n", rdid);
		return &snand_info;
	}
	else
	{
		printf("\nNo recognized ID, rdid=0x%x\n", rdid);
	}

	return VZERO;
	// return CMD_RET_SUCCESS;
}

void snand_set_pinmux(void)
{
	// first set pinmux for spi nand
	REG32(0xB8000818) = 0x33000000;
	REG32(0xB800081C) = 0x33030000;
	REG32(0xB8000014) = 0xffffffff;
}

#if CONF_SPI_NAND_UBOOT_COMMAND
static int
probe_snand_spi_nand_chip_uboot(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	// dram_normal_patterns();

#if 1 //stevie todo
	  // first set pinmux for spi nor
	  snand_set_pinmux();
#endif

	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	// SNAND_ChipSelect(chip_select);
#endif
	SNAND_ChipSelect(chip_select);
	// flash_EN4B(&dev);
	probe_snand_chip();

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(snwb, 1, 0, probe_snand_spi_nand_chip_uboot,
           "snwb  : Probe SPI NAND for Winbond series \n", "");
#endif


#if CONF_SPI_NAND_UBOOT_COMMAND
static int
SPI_NAND_RDID(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]
)
{
	// dram_normal_patterns();


	unsigned int spi_nand_id = 0x0;
	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	// SNAND_ChipSelect(chip_select);
#endif
	SNAND_ChipSelect(chip_select);

#if 0
	if (argc < 2)
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snid<chip_select>\r\n");

		return CMD_RET_FAILURE;
	}
#endif


	// flash_EN4B(&dev);
	spi_nand_id = snand_read_id();

	printf("\nGet spi_nand_id =0x%x\n", spi_nand_id);

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(snid, 1, 0, SPI_NAND_RDID,
           "snid  : Send RDID(0x9f) and get SPI-NAND ID  \n", "");
#endif

#if CONF_SPI_NAND_UBOOT_COMMAND
static int
SPI_NAND_RESET(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]
)
{
	// dram_normal_patterns();

#if 1 //stevie todo
	  // first set pinmux for spi nor
	REG32(0xB8000818) = 0x33000000;
	REG32(0xB800081C) = 0x33030000;
	REG32(0xB8000014) = 0xffffffff;
#endif
	//unsigned int spi_nand_id = 0x0;
	/*
	 * Select cs0 or cs1 for user mode , auto mode's setting is
	 * FLASH_SIZE(offset + 0x124)
	 */
#if 0
	u32 chip_select = Strtoul((const u8 *) (argv[1]), (u8 **) NULL, 16);
	// SNAND_ChipSelect(chip_select);
#endif
	SNAND_ChipSelect(chip_select);
#if 0
	if (argc < 2)
	{
		printf("Wrong argument number!\r\n");
		printf("Usage:snid<chip_select>\r\n");


		return CMD_RET_FAILURE;
	}


#endif


	// flash_EN4B(&dev);
	snand_reset();

	printf("\nReset SPI nand\n");

	return CMD_RET_SUCCESS;
}



U_BOOT_CMD(snreset, 1, 0, SPI_NAND_RESET,
           "snid  : Send RESET(0xff) and get SPI-NAND ID  \n", "");
#endif

// REG_SPI_NAND_PROBE_FUNC(probe_snand_spi_nand_chip);

